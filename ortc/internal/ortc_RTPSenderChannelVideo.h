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
#include <ortc/internal/ortc_RTPSenderChannelMediaBase.h>

#include <ortc/IRTPTypes.h>

#include <openpeer/services/IWakeDelegate.h>
#include <zsLib/MessageQueueAssociator.h>
#include <zsLib/Timer.h>

//#include <webrtc/base/scoped_ptr.h>
//#include <webrtc/modules/utility/interface/process_thread.h>
//#include <webrtc/Transport.h>
//#include <webrtc/video/transport_adapter.h>
//#include <webrtc/video_engine/vie_channel_group.h>
//#include <webrtc/video_send_stream.h>
//#include <webrtc/modules/video_capture/include/video_capture.h>


//#define ORTC_SETTING_SCTP_TRANSPORT_MAX_MESSAGE_SIZE "ortc/sctp/max-message-size"

namespace ortc
{
  namespace internal
  {
    ZS_DECLARE_INTERACTION_PTR(IRTPSenderChannelVideoForSettings)
    ZS_DECLARE_INTERACTION_PTR(IRTPSenderChannelMediaBaseForRTPSenderChannel)
    ZS_DECLARE_INTERACTION_PTR(IRTPSenderChannelVideoForRTPSenderChannel)
    ZS_DECLARE_INTERACTION_PTR(IRTPSenderChannelMediaBaseForMediaStreamTrack)
    ZS_DECLARE_INTERACTION_PTR(IRTPSenderChannelVideoForMediaStreamTrack)

    ZS_DECLARE_INTERACTION_PTR(IRTPSenderChannelForRTPSenderChannelMediaBase)
    ZS_DECLARE_INTERACTION_PTR(IRTPSenderChannelForRTPSenderChannelVideo)
    ZS_DECLARE_INTERACTION_PTR(IMediaStreamTrackForRTPSenderChannelMediaBase)
    ZS_DECLARE_INTERACTION_PTR(IMediaStreamTrackForRTPSenderChannelVideo)

    ZS_DECLARE_INTERACTION_PROXY(IRTPSenderChannelVideoAsyncDelegate)

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark IRTPSenderChannelVideoVideoForSettings
    #pragma mark

    interaction IRTPSenderChannelVideoForSettings
    {
      ZS_DECLARE_TYPEDEF_PTR(IRTPSenderChannelVideoForSettings, ForSettings)

      static void applyDefaults();

      virtual ~IRTPSenderChannelVideoForSettings() {}
    };

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark IRTPSenderChannelVideoForRTPSenderChannel
    #pragma mark

    interaction IRTPSenderChannelVideoForRTPSenderChannel : public IRTPSenderChannelMediaBaseForRTPSenderChannel
    {
      ZS_DECLARE_TYPEDEF_PTR(IRTPSenderChannelVideoForRTPSenderChannel, ForRTPSenderChannel)

      ZS_DECLARE_TYPEDEF_PTR(IRTPTypes::Parameters, Parameters)

      static RTPSenderChannelVideoPtr create(
                                             RTPSenderChannelPtr senderChannel,
                                             const Parameters &params
                                             );
    };

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark IRTPSenderChannelVideoForMediaStreamTrack
    #pragma mark

    interaction IRTPSenderChannelVideoForMediaStreamTrack : public IRTPSenderChannelMediaBaseForMediaStreamTrack
    {
      ZS_DECLARE_TYPEDEF_PTR(IRTPSenderChannelVideoForMediaStreamTrack, ForMediaStreamTrack)
    };

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark IRTPSenderChannelVideoAsyncDelegate
    #pragma mark

    interaction IRTPSenderChannelVideoAsyncDelegate
    {
      virtual ~IRTPSenderChannelVideoAsyncDelegate() {}
    };

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark RTPSenderChannelVideo
    #pragma mark
    
    class RTPSenderChannelVideo : public Noop,
                                  public MessageQueueAssociator,
                                  public SharedRecursiveLock,
                                  public IRTPSenderChannelVideoForSettings,
                                  public IRTPSenderChannelVideoForRTPSenderChannel,
                                  public IRTPSenderChannelVideoForMediaStreamTrack,
                                  public IWakeDelegate,
                                  public zsLib::ITimerDelegate,
                                  public IRTPSenderChannelVideoAsyncDelegate
    {
    protected:
      struct make_private {};

    public:
      friend interaction IRTPSenderChannelVideo;
      friend interaction IRTPSenderChannelVideoFactory;
      friend interaction IRTPSenderChannelVideoForSettings;
      friend interaction IRTPSenderChannelMediaBaseForRTPSenderChannel;
      friend interaction IRTPSenderChannelVideoForRTPSenderChannel;
      friend interaction IRTPSenderChannelMediaBaseForMediaStreamTrack;
      friend interaction IRTPSenderChannelVideoForMediaStreamTrack;

      ZS_DECLARE_TYPEDEF_PTR(IRTPSenderChannelForRTPSenderChannelVideo, UseChannel)
      ZS_DECLARE_TYPEDEF_PTR(IMediaStreamTrackForRTPSenderChannelVideo, UseMediaStreamTrack)

      ZS_DECLARE_TYPEDEF_PTR(IRTPSenderChannelMediaBaseForRTPSenderChannel, ForRTPSenderChannelFromMediaBase)
      ZS_DECLARE_TYPEDEF_PTR(IRTPSenderChannelMediaBaseForMediaStreamTrack, ForMediaStreamTrackFromMediaBase)

      ZS_DECLARE_TYPEDEF_PTR(IRTPTypes::Parameters, Parameters)
      typedef std::list<RTCPPacketPtr> RTCPPacketList;
      ZS_DECLARE_PTR(RTCPPacketList)

      enum States
      {
        State_Pending,
        State_Ready,
        State_ShuttingDown,
        State_Shutdown,
      };
      static const char *toString(States state);

    public:
      RTPSenderChannelVideo(
                            const make_private &,
                            IMessageQueuePtr queue,
                            UseChannelPtr senderChannel,
                            const Parameters &params
                            );

    protected:
      RTPSenderChannelVideo(Noop, IMessageQueuePtr queue = IMessageQueuePtr()) :
        Noop(true),
        MessageQueueAssociator(queue),
        SharedRecursiveLock(SharedRecursiveLock::create())
      {}

      void init();

    public:
      virtual ~RTPSenderChannelVideo();

      static RTPSenderChannelVideoPtr convert(ForSettingsPtr object);
      static RTPSenderChannelVideoPtr convert(ForRTPSenderChannelFromMediaBasePtr object);
      static RTPSenderChannelVideoPtr convert(ForRTPSenderChannelPtr object);
      static RTPSenderChannelVideoPtr convert(ForMediaStreamTrackFromMediaBasePtr object);
      static RTPSenderChannelVideoPtr convert(ForMediaStreamTrackPtr object);

    protected:
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPSenderChannelVideo => IRTPSenderChannelMediaBaseForRTPSenderChannel
      #pragma mark

      virtual PUID getID() const override {return mID;}

      virtual bool handlePacket(RTCPPacketPtr packet) override;

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPSenderChannelVideo => IRTPSenderChannelVideoForRTPSenderChannel
      #pragma mark

      static RTPSenderChannelVideoPtr create(
                                             RTPSenderChannelPtr senderChannel,
                                             const Parameters &params
                                             );

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPSenderChannelVideo => IRTPSenderChannelMediaBaseForMediaStreamTrack
      #pragma mark

      // (duplicate) virtual PUID getID() const = 0;

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPSenderChannelVideo => IRTPSenderChannelVideoForMediaStreamTrack
      #pragma mark

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPSenderChannelVideo => IWakeDelegate
      #pragma mark

      virtual void onWake() override;

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPSenderChannelVideo => ITimerDelegate
      #pragma mark

      virtual void onTimer(TimerPtr timer) override;

      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPSenderChannelVideo => IRTPSenderChannelVideoAsyncDelegate
      #pragma mark

    protected:
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark RTPSenderChannelVideo => (internal)
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
      #pragma mark RTPSenderChannelVideo => (data)
      #pragma mark

      AutoPUID mID;
      RTPSenderChannelVideoWeakPtr mThisWeak;
      RTPSenderChannelVideoPtr mGracefulShutdownReference;

      States mCurrentState {State_Pending};

      WORD mLastError {};
      String mLastErrorReason;

      UseChannelWeakPtr mSenderChannel;

      ParametersPtr mParameters;
    };

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark IRTPSenderChannelVideoFactory
    #pragma mark

    interaction IRTPSenderChannelVideoFactory
    {
      ZS_DECLARE_TYPEDEF_PTR(IRTPTypes::Parameters, Parameters)

      static IRTPSenderChannelVideoFactory &singleton();

      virtual RTPSenderChannelVideoPtr create(
                                              RTPSenderChannelPtr sender,
                                              const Parameters &params
                                              );
    };

    class RTPSenderChannelVideoFactory : public IFactory<IRTPSenderChannelVideoFactory> {};
  }
}

ZS_DECLARE_PROXY_BEGIN(ortc::internal::IRTPSenderChannelVideoAsyncDelegate)
ZS_DECLARE_PROXY_END()