﻿//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

#include <zsLib\Singleton.h>
#include <ortc\test\testing.h>

using namespace ortclibTest;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

Windows::UI::Core::CoreDispatcher^ g_windowDispatcher;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();
}

void ortclibTest::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
  g_windowDispatcher = Window::Current->Dispatcher;
  RunTestButton->IsEnabled = false;
  Concurrency::create_task([this]() {
    TESTING_STDOUT() << "TEST NOW STARTING...\n\n";

    IInspectable* localVideoMediaElement = reinterpret_cast<IInspectable*>(LocalVideoMediaElement);
    IInspectable* remoteVideoMediaElement = reinterpret_cast<IInspectable*>(RemoteVideoMediaElement);
    Testing::runAllTests(localVideoMediaElement, remoteVideoMediaElement);
    Testing::output();

    if (0 != Testing::getGlobalFailedVar()) {
      TESTING_STDOUT() << "FAILED!\n\n";
    }

    return Concurrency::create_task(g_windowDispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]() {
      RunTestButton->IsEnabled = true;
    })));
  });
}
