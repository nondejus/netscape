/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.0 (the "NPL"); you may not use this file except in
 * compliance with the NPL.  You may obtain a copy of the NPL at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the NPL is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the NPL
 * for the specific language governing rights and limitations under the
 * NPL.
 *
 * The Initial Developer of this code under the NPL is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation.  All Rights
 * Reserved.
 */

// ���������������������������������������������������������������������������
//	CDragBar.cp
// ���������������������������������������������������������������������������

#pragma once

#include "CDragBar.h"
#include "CDragBarDragTask.h"
#include "CSharedPatternWorld.h"
#include "UGraphicGizmos.h"
#include "CPatternedGrippyPane.h"


// ���������������������������������������������������������������������������
//	�	
// ���������������������������������������������������������������������������

CDragBar::CDragBar(LStream* inStream)
	:	LView(inStream)
{
	mPatternWorld = CSharedPatternWorld::CreateSharedPatternWorld(10000);
	ThrowIfNULL_(mPatternWorld);
	mPatternWorld->AddUser(this);
	
	inStream->ReadPString(mTitle);
	*inStream >> mIsDocked;

	::SetEmptyRgn(mDockedMask);
	
	mIsTracking = false;
	// The visible flag will be off or latent at this point.  If it's invisible in the
	// resource, we want to mark it as fully hidden (ie, not docked either)
	mIsAvailable = (mVisible != triState_Off);
	SetRefreshAllWhenResized(false);
}

// ���������������������������������������������������������������������������
//	�	
// ���������������������������������������������������������������������������

CDragBar::~CDragBar()
{
	mPatternWorld->RemoveUser(this);
	::SetEmptyRgn(mDockedMask);
}

// ���������������������������������������������������������������������������
//	�	
// ���������������������������������������������������������������������������

void CDragBar::Dock(void)
{
	mIsDocked = true;
	Hide();
}

// ���������������������������������������������������������������������������
//	�	
// ���������������������������������������������������������������������������

void CDragBar::Undock(void)
{
	mIsDocked = false;
	Show();
}

// ���������������������������������������������������������������������������
//	�	
// ���������������������������������������������������������������������������

Boolean CDragBar::IsDocked(void) const
{
	return mIsDocked;
}

// ���������������������������������������������������������������������������
//	�	
// ���������������������������������������������������������������������������

void CDragBar::StartTracking(void)
{
	mIsTracking = true;
	Draw(NULL);
}

// ���������������������������������������������������������������������������
//	�	
// ���������������������������������������������������������������������������

void CDragBar::StopTracking(void)
{
	mIsTracking = false;
	Draw(NULL);
}

// ���������������������������������������������������������������������������
//	�	
// ���������������������������������������������������������������������������

StringPtr CDragBar::GetDescriptor(Str255 outDescriptor) const
{
	LString::CopyPStr(mTitle, outDescriptor);
	return outDescriptor;
}

// ���������������������������������������������������������������������������
//	�	
// ���������������������������������������������������������������������������

void CDragBar::SetDescriptor(ConstStringPtr inDescriptor)
{
	mTitle = inDescriptor;
}


// ���������������������������������������������������������������������������
//	�	
// ���������������������������������������������������������������������������

void CDragBar::Draw(RgnHandle inSuperDrawRgnH)
{
	// Don't draw if invisible or unable to put in focus
	if (IsVisible() && FocusDraw())
	{
		// Area of this View to draw is the intersection of inSuperDrawRgnH
		// with the Revealed area of this View
		RectRgn(mUpdateRgnH, &mRevealedRect);
		if (inSuperDrawRgnH != nil)
			SectRgn(inSuperDrawRgnH, mUpdateRgnH, mUpdateRgnH);
			
		if (!EmptyRgn(mUpdateRgnH))			// Some portion needs to be drawn
		{
									
			Rect frame;
			CalcLocalFrameRect(frame);
						
			// A View is visually behind its SubPanes so it draws itself first,
			//then its SubPanes			
			if (ExecuteAttachments(msg_DrawOrPrint, &frame))
				DrawSelf();

			if (!mIsTracking)
			{
				LArrayIterator iterator(mSubPanes, LArrayIterator::from_Start);
				LPane	*theSub;
				while (iterator.Next(&theSub))
					theSub->Draw(mUpdateRgnH);
			}
		}
		
		::SetEmptyRgn(mUpdateRgnH);	// Emptying update region frees up memory
									// if this region wasn't rectangular
	}
}

// ���������������������������������������������������������������������������
//	�	
// ���������������������������������������������������������������������������

void CDragBar::Click(SMouseDownEvent& inMouseDown)
{
	LPane* theClickedPane = FindDeepSubPaneContaining(inMouseDown.wherePort.h, inMouseDown.wherePort.v );
	if ((theClickedPane != NULL) && (theClickedPane->GetPaneID() == CPatternedGrippyPane::class_ID))
		{
		ClickDragSelf(inMouseDown);
		}
	else
		{
		theClickedPane = FindSubPaneHitBy(inMouseDown.wherePort.h, inMouseDown.wherePort.v);
		if (theClickedPane != NULL)
			theClickedPane->Click(inMouseDown);
		}
}

// ���������������������������������������������������������������������������
//	� DragBars may be hidden from container and dock by javascript - mjc
// ���������������������������������������������������������������������������

void CDragBar::SetAvailable(Boolean inAvailable)
{
	mIsAvailable = inAvailable;
}

Boolean CDragBar::IsAvailable()
{
	return mIsAvailable;
}

// ���������������������������������������������������������������������������
//	�	
// ���������������������������������������������������������������������������

void CDragBar::DrawSelf(void)
{
	StColorPenState::Normalize();
	
	Rect theFrame;
	CalcLocalFrameRect(theFrame);
	
	Point theAlignment;
	CSharedPatternWorld::CalcRelativePoint(this, CSharedPatternWorld::eOrientation_Port, theAlignment);

	// We do this instead of LPane::GetMacPort() because we may be being
	// drawn offscreen.
	CGrafPtr thePort;
	::GetPort(&(GrafPtr)thePort);
	mPatternWorld->Fill(thePort, theFrame, theAlignment);

	if (mIsTracking)
		UGraphicGizmos::LowerColorVolume(theFrame, 0x2000);
}

// ���������������������������������������������������������������������������
//	�	
// ���������������������������������������������������������������������������

void CDragBar::ClickDragSelf(const SMouseDownEvent& inMouseDown)
{
	FocusDraw();
	
	Point theGlobalPoint = inMouseDown.wherePort;
	PortToGlobalPoint(theGlobalPoint);
	if (::WaitMouseMoved(theGlobalPoint))
		{
		CDragBarDragTask theTask(this, inMouseDown.macEvent);
		theTask.DoDrag();
		}
	else
		BroadcastMessage(msg_DragBarCollapse, this);
}