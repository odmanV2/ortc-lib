/*
 
 Copyright (c) 2013, SMB Phone Inc. / Hookflash Inc.
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
#include <ortc/IMediaStream.h>

namespace ortc
{
  namespace internal
  {
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    #pragma mark
    #pragma mark IMediaStreamForMediaManager
    #pragma mark
    
    interaction IMediaStreamForMediaManager
    {
      IMediaStreamForMediaManager &forMediaManager() {return *this;}
      const IMediaStreamForMediaManager &forMediaManager() const {return *this;}
      
      static MediaStreamPtr create(
                                   IMessageQueuePtr queue,
                                   IMediaStreamDelegatePtr delegate
                                   );
      
    };
    
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    #pragma mark
    #pragma mark IMediaStreamForRTCConnection
    #pragma mark
    
    interaction IMediaStreamForRTCConnection
    {
      IMediaStreamForRTCConnection &forRTCConnection() {return *this;}
      const IMediaStreamForRTCConnection &forRTCConnection() const {return *this;}
    };
    
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    #pragma mark
    #pragma mark MediaStream
    #pragma mark
    
    class MediaStream : public Noop,
      public MessageQueueAssociator,
      public IMediaStream,
      public IMediaStreamForMediaManager,
      public IMediaStreamForRTCConnection
    {
    public:
      friend interaction IMediaStream;
      friend interaction IMediaStreamForMediaManager;
      friend interaction IMediaStreamForRTCConnection;
      
    protected:
      MediaStream(
                  IMessageQueuePtr queue,
                  IMediaStreamDelegatePtr delegate
                  );
      
    public:
      virtual ~MediaStream();
      
      //---------------------------------------------------------------------
      #pragma mark
      #pragma mark MediaStream => IMediaStream
      #pragma mark
      
    protected:
      virtual String id();
      virtual MediaStreamTrackListPtr getAudioTracks();
      virtual MediaStreamTrackListPtr getVideoTracks();
      virtual IMediaStreamTrackPtr getTrackById(String trackId);
      virtual void addTrack(IMediaStreamTrackPtr track);
      virtual void removeTrack(IMediaStreamTrackPtr track);
      virtual IMediaStreamPtr clone();
      virtual bool inactive();
      
      //---------------------------------------------------------------------
      #pragma mark
      #pragma mark MediaStream => IMediaStreamForMediaManager
      #pragma mark
      
      //---------------------------------------------------------------------
      #pragma mark
      #pragma mark MediaStream => IMediaStreamForRTCConnection
      #pragma mark
      
      //---------------------------------------------------------------------
      #pragma mark
      #pragma mark MediaStream => (internal)
      #pragma mark
      
      //---------------------------------------------------------------------
      #pragma mark
      #pragma mark MediaStream => (data)
      #pragma mark
      
    protected:
      PUID mID;
      mutable RecursiveLock mLock;
      MediaStreamWeakPtr mThisWeak;
      IMediaStreamDelegatePtr mDelegate;
      
      int mError;
      
      String mStreamID;
      MediaStreamTrackListPtr mAudioTracks;
      MediaStreamTrackListPtr mVideoTracks;
      bool mInactive;
    };
  }
}
