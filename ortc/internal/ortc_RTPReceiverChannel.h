/*

 Copyright (c) 2015, Hookflash Inc. / Hookflash Inc.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 The views and conclusions contained in the software and documentation are those
 of the authors and should not be interpreted as representing official policies,
 either expressed or implied, of the FreeBSD Project.
 
 */

#pragma once

#include <ortc/internal/types.h>

#include <ortc/IICETransport.h>
#include <ortc/IDTLSTransport.h>
#include <ortc/IRTPTypes.h>

#include <openpeer/services/IWakeDelegate.h>
#include <zsLib/MessageQueueAssociator.h>
#include <zsLib/Timer.h>


//#define ORTC_SETTING_SCTP_TRANSPORT_MAX_MESSAGE_SIZE "ortc/sctp/max-message-size"

namespace ortc
{
  namespace internal
  {
    ZS_DECLARE_INTERACTION_PTR(IRTPReceiverChannelForSettings)
    ZS_DECLARE_INTERACTION_PTR(IRTPReceiverChannelForRTPReceiver)

    ZS_DECLARE_INTERACTION_PTR(IRTPReceiverForRTPReceiverChannel)

    ZS_DECLARE_INTERACTION_PROXY(IRTPReceiverChannelAsyncDelegate)

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark IRTPReceiverChannelForSettings
    #pragma mark

    interaction IRTPReceiverChannelForSettings
    {
      ZS_DECLARE_TYPEDEF_PTR(IRTPReceiverChannelForSettings, ForSettings)

      static void applyDefaults();

      virtual ~IRTPReceiverChannelForSettings() {}
    };
    
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark IRTPReceiverChannelForRTPReceiver
    #pragma mark

    interaction IRTPReceiverChannelForRTPReceiver
    {
      ZS_DECLARE_TYPEDEF_PTR(IRTPReceiverChannelForRTPReceiver, ForRTPReceiver)

      ZS_DECLARE_TYPEDEF_PTR(IRTPTypes::Parameters, Parameters)

      static ElementPtr toDebug(ForRTPReceiverPtr object);

      static RTPReceiverChannelPtr create(
                                          RTPReceiverPtr receiver,
                                          const Parameters &params
                                          );

      virtual PUID getID() const = 0;

      virtual void update(const Parameters &params);

      virtual bool handlePacket(RTPPacketPtr packet) = 0;

      virtual bool handlePacket(RTCPPacketPtr packet) = 0;
    };

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark IRTPReceiverChannelAsyncDelegate
    #pragma mark

    interaction IRTPReceiverChannelAsyncDelegate
    {
      virtual ~IRTPReceiverChannelAsyncDelegate() {}
    };

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark RTPReceiverChannel
    #pragma mark
    
    class RTPReceiverChannel : public Noop,
                               public MessageQueueAssociator,
                               public SharedRecursiveLock,
                               public IRTPReceiverChannelForSettings,
                               public IRTPReceiverChannelForRTPReceiver,
                               public IWakeDelegate,
                               public zsLib::ITimerDelegate,
                               public IRTPReceiverChannelAsyncDelegate
    {
    protected:
      struct make_private {};

    public:
      friend interaction IRTPReceiverChannel;
      friend interaction IRTPReceiverChannelFactory;
      friend interaction IRTPReceiverChannelForSettings;
      friend interaction IRTPReceiverChannelForRTPReceiver;

      ZS_DECLARE_TYPEDEF_PTR(IRTPReceiverForRTPReceiverChannel, UseReceiver)
      ZS_DECLARE_TYPEDEF_PTR(IRTPTypes::Parameters, Parameters)

      enum States
      {
        State_Pending,
        State_Ready,
        State_ShuttingDown,
        State_Shutdown,
      };
      static const char *toString(States state);

    public:
      RTPReceiverChannel(
                         const make_private &,
                         IMessageQueuePtr queue,
                         UseReceiverPtr receiver,
                         const Parameters &params
                         );

    protected:
      RTPReceiverChannel(Noop) :
        Noop(true),
        MessageQueueAssociator(IMessageQueuePtr()),
        SharedRecursiveLock(SharedRecursiveLock::create())
      {}

      void init();

    public:
      virtual ~RTPReceiverChannel();

      static RTPReceiverChannelPtr convert(ForSettingsPtr object);
      static RTPReceiverChannelPtr convert(ForRTPReceiverPtr object);

    protected:
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPReceiverChannel => ForRTPReceiver
      #pragma mark

      static ElementPtr toDebug(RTPReceiverChannelPtr object);

      static RTPReceiverChannelPtr create(
                                          RTPReceiverPtr receiver,
                                          const Parameters &params
                                          );

      virtual PUID getID() const override {return mID;}

      virtual void update(const Parameters &params) override;

      virtual bool handlePacket(RTPPacketPtr packet) override;

      virtual bool handlePacket(RTCPPacketPtr packet) override;

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPReceiverChannel => IWakeDelegate
      #pragma mark

      virtual void onWake() override;

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPReceiverChannel => ITimerDelegate
      #pragma mark

      virtual void onTimer(TimerPtr timer) override;

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPReceiverChannel => IRTPReceiverChannelAsyncDelegate
      #pragma mark

    protected:
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPReceiverChannel => (internal)
      #pragma mark

      Log::Params log(const char *message) const;
      Log::Params debug(const char *message) const;
      virtual ElementPtr toDebug() const;

      bool isShuttingDown() const;
      bool isShutdown() const;

      void step();
      bool stepBogusDoSomething();

      void cancel();

      void setState(States state);
      void setError(WORD error, const char *reason = NULL);


    protected:
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPReceiverChannel => (data)
      #pragma mark

      AutoPUID mID;
      RTPReceiverChannelWeakPtr mThisWeak;
      RTPReceiverChannelPtr mGracefulShutdownReference;


      States mCurrentState {State_Pending};

      WORD mLastError {};
      String mLastErrorReason;

      ParametersPtr mParameters;
      
      UseReceiverPtr mReceiver;
    };

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark IRTPReceiverChannelFactory
    #pragma mark

    interaction IRTPReceiverChannelFactory
    {
      ZS_DECLARE_TYPEDEF_PTR(IRTPTypes::Parameters, Parameters)

      static IRTPReceiverChannelFactory &singleton();

      virtual RTPReceiverChannelPtr create(
                                           RTPReceiverPtr receiver,
                                           const Parameters &params
                                           );
    };

    class RTPReceiverChannelFactory : public IFactory<IRTPReceiverChannelFactory> {};
  }
}

ZS_DECLARE_PROXY_BEGIN(ortc::internal::IRTPReceiverChannelAsyncDelegate)
//ZS_DECLARE_PROXY_METHOD_0(onWhatever)
ZS_DECLARE_PROXY_END()