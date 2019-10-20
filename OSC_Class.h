/*
  ==============================================================================

   This file is part of the JUCE examples.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             OSCDemo
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Application using the OSC protocol.

 dependencies:     juce_core, juce_data_structures, juce_events, juce_graphics,
				   juce_gui_basics, juce_osc
 exporters:        xcode_mac, vs2017, linux_make

 type:             Component
 mainClass:        OSCDemo

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================

//==============================================================================
class OSCReceiverDemo : public Component,
	private OSCReceiver,
	private OSCReceiver::ListenerWithOSCAddress<OSCReceiver::MessageLoopCallback>
{
public:
	//==============================================================================
	OSCReceiverDemo()
	{
		
		// specify here on which UDP port number to receive incoming OSC messages
		if (!connect(9999))
			showConnectionErrorMessage("Error: could not connect to UDP port 9999.");

		// tell the component to listen for OSC messages matching this address:
		addListener(this, "/fsr");
	}

	bool messageReady = false;

	float oscDataArray[10] = {0.0};

private:
	//==============================================================================
	void oscMessageReceived(const OSCMessage& message) override
	{
		messageReady = false;
		
		auto it = message.begin();

		for (int i = 0; i < message.size(); i++)
		{
			if (message.size() == 6 && message[i].isFloat32())
			{
				oscDataArray[i] = jlimit(0.0f, 10000.0f, message[i].getFloat32());
			}
		}
		messageReady = true;
	}
	void showConnectionErrorMessage(const String& messageText)
	{
		AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
			"Connection error",
			messageText,
			"OK");
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCReceiverDemo)
};


