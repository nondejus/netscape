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

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include <LView.h>
#include <UDrawingState.h>

#include "CPatternProgressBar.h"
#include "CSharedPatternWorld.h"
#include "CGWorld.h"
#include "UGraphicGizmos.h"
#ifdef MOZ_OFFLINE
#include "UOffline.h"
#endif

const ResIDT cOfflineListID = 16010;
const Int16 cOfflineStrIndex = 3;

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//
#pragma mark --- CPatternProgressBar ---
//
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

CPatternProgressBar::CPatternProgressBar(LStream* inStream)
	:	LPane(inStream)
{
	ResIDT thePatternID;
	*inStream >> thePatternID;
	*inStream >> mOrientation;
			
	mPatternWorld = CSharedPatternWorld::CreateSharedPatternWorld(thePatternID);
	ThrowIfNULL_(mPatternWorld);
	mPatternWorld->AddUser(this);

	mPatOffset = 0;
	mValue = eSeamless;
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

CPatternProgressBar::~CPatternProgressBar()
{
	mPatternWorld->RemoveUser(this);
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void CPatternProgressBar::FinishCreateSelf(void)
{
	LPane::FinishCreateSelf();
//	SetValueRange(Range32T(0,100));
	mValueRange = 100;
	RecalcPoleMasks();
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void CPatternProgressBar::ActivateSelf(void)
{
	if (FocusExposed())
		Refresh(); //Draw(NULL);
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void CPatternProgressBar::DeactivateSelf(void)
{
	if (FocusExposed())
		Refresh(); //Draw(NULL);
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void CPatternProgressBar::DrawSelf(void)
{
	Rect theFrame;
	CalcLocalFrameRect(theFrame);
	
	Int32 theValue = GetValue();
	if (theValue == eIndefinite)
		DrawIndefiniteBar(theFrame);
	else if (theValue == eSeamless)
		DrawSeamlessBar(theFrame);
	else
		DrawPercentageBar(theFrame);
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void CPatternProgressBar::DrawIndefiniteBar(const Rect& inBounds)
{
	Rect theBoundsCopy = inBounds;

	// First we need to fill, bevel and recess the bar area
	Point theAlignment;
	CSharedPatternWorld::CalcRelativePoint(this, CSharedPatternWorld::eOrientation_Port, theAlignment);

	CGrafPtr thePort;
	::GetPort(&(GrafPtr)thePort);
	mPatternWorld->Fill(thePort, inBounds, theAlignment);
	UGraphicGizmos::BevelTintRect(theBoundsCopy, -1, 0x4000, 0x4000);
	::InsetRect(&theBoundsCopy, 1, 1);
	UGraphicGizmos::LowerColorVolume(theBoundsCopy, 0x2000);

	// Get our drawing port, and save the origin state.
	// Don't assume we are onscreen (GetMacPort() would do that).
	StPortOriginState theOriginSaver((GrafPtr)thePort);

	// fake the pattern into thinking it's drawn somewhere else	
	Point theOrigin;
	theOrigin.h = thePort->portRect.left;
	theOrigin.v = thePort->portRect.top;
	::SetOrigin(theOrigin.h - mPatOffset, theOrigin.v);

	// Adjust the drawing frame for the shift in port origin
	::OffsetRect(&theBoundsCopy, -mPatOffset, 0);

	// changing the origion means that we must offset the clip region
	StClipRgnState theClipSaver;
	::ClipRect(&theBoundsCopy);

	RGBColor theFillPin = { 0x2000, 0x2000, 0x2000 };
	::RGBForeColor(&theFillPin);
	::OpColor(&UGraphicGizmos::sLighter);
	::PenMode(addPin);
	::PaintRgn(mFillMask);

	RGBColor theBevelPin = { 0x4000, 0x4000, 0x4000 };
	::RGBForeColor(&theBevelPin);
	::PaintRgn(mTopBevelMask);
	
	::OpColor(&UGraphicGizmos::sDarker);
	::PenMode(subPin);
	::PaintRgn(mBottomBevelMask);

	mPatOffset = (mPatOffset + 1) % mPatWidth;
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void CPatternProgressBar::DrawSeamlessBar(const Rect& inBounds)
{
#if 1
	mValue = mValueRange;
	DrawPercentageBar(inBounds);
#else
	Point theAlignment;
	CSharedPatternWorld::CalcRelativePoint(this, CSharedPatternWorld::eOrientation_Port, theAlignment);

	CGrafPtr thePort;
	::GetPort(&(GrafPtr)thePort);
	mPatternWorld->Fill(thePort, inBounds, theAlignment);
#endif
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void CPatternProgressBar::DrawPercentageBar(const Rect& inBounds)
{
	Int32 theStart = 0;
	Int32 theEnd = mValueRange;

	Int32 theValue = GetValue();
	if (theValue < theStart)
		theValue = theStart;
	else if (theValue > theEnd)
		theValue = theEnd;
		
	Int32 thePercentage = ((theValue * 100) / theEnd);

	Rect theBoundsCopy = inBounds;
	::InsetRect(&theBoundsCopy, 1, 1);
	
	Rect theLeftBox = theBoundsCopy;
	theLeftBox.right = theLeftBox.left + (thePercentage * RectWidth(theBoundsCopy)) / 100;
	
	Rect theRightBox = theBoundsCopy;
	theRightBox.left = theLeftBox.right;
	
	Point theAlignment;
	CSharedPatternWorld::CalcRelativePoint(this, CSharedPatternWorld::eOrientation_Port, theAlignment);

	CGrafPtr thePort;
	::GetPort(&(GrafPtr)thePort);

	mPatternWorld->Fill(thePort, inBounds, theAlignment);
	UGraphicGizmos::BevelTintRect(inBounds, -1, 0x4000, 0x6000);
	
	if (RectWidth(theLeftBox) >= 5)
		{
		UGraphicGizmos::BevelTintRect(theLeftBox, 1, 0x4000, 0x6000);
		::InsetRect(&theLeftBox, 1, 1);
		UGraphicGizmos::LowerColorVolume(theLeftBox, 0x1000);
		}
	else
		theRightBox.left = theLeftBox.left;
	
	UGraphicGizmos::LowerColorVolume(theRightBox, 0x3000);
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

Int32 CPatternProgressBar::GetValue() const
{
	return mValue;
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void CPatternProgressBar::SetValue(Int32 inValue)
{
	if ((inValue != mValue) || (inValue == eIndefinite))
		{
		mValue = inValue;
		if (FocusExposed())
			Draw(NULL);
		}
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
void CPatternProgressBar::SetValueRange(UInt32 inValueRange)
{
	mValueRange = inValueRange;
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void CPatternProgressBar::Draw(RgnHandle inSuperDrawRgnH)
{
	Rect theFrame;
	if ((mVisible == triState_On) && CalcPortFrameRect(theFrame) &&
			((inSuperDrawRgnH == nil) || RectInRgn(&theFrame, inSuperDrawRgnH)) && FocusDraw())
		{
		PortToLocalPoint(topLeft(theFrame));	// Get Frame in Local coords
		PortToLocalPoint(botRight(theFrame));

		if (ExecuteAttachments(msg_DrawOrPrint, &theFrame))
			{
			Boolean bDidDraw = false;

			StColorPenState thePenSaver;
			StColorPenState::Normalize();
			
			// Fail safe offscreen drawing
			StValueChanger<EDebugAction> okayToFail(gDebugThrow, debugAction_Nothing);
			try
				{			
				LGWorld theOffWorld(theFrame, 0, useTempMem);

				if (!theOffWorld.BeginDrawing())
					throw memFullErr;
					
				DrawSelf();
					
				theOffWorld.EndDrawing();
				theOffWorld.CopyImage(GetMacPort(), theFrame);
				bDidDraw = true;
				}
			catch (...)
				{
				// 	& draw onscreen
				}

			if (!bDidDraw)
				DrawSelf();
			}
		}
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯	
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void CPatternProgressBar::ResizeFrameBy(
	Int16 			inWidthDelta,
	Int16			inHeightDelta,
	Boolean			inRefresh)
{
	LPane::ResizeFrameBy(inWidthDelta, inHeightDelta, inRefresh);
	RecalcPoleMasks();
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void CPatternProgressBar::RecalcPoleMasks(void)
{
	StColorPenState::Normalize();

	Rect theFrame;
	CalcLocalFrameRect(theFrame);
	::InsetRect(&theFrame, 1, 1);
	theFrame.right += RectHeight(theFrame) * 2;
	theFrame.left -= RectHeight(theFrame) * 2;
	
	// mjc begin
	// handle case where bar is resized to zero or negative width (in narrow windows).
	// Make the mask one pixel wide so we don't throw on creating a mask with negative width.
	if (theFrame.right <= theFrame.left) theFrame.right = theFrame.left + 1;
	// mjc end
	
	CGWorld theMaskWorld(theFrame, 1, useTempMem);

	Int16 theHeight = RectHeight(theFrame);
	Int16 theHalfHeight = RectHeight(theFrame) / 2;
	
	Rect theStamp = theFrame;
	theStamp.right = theStamp.left + RectHeight(theStamp) + theHalfHeight;

	StRegion theStampRgn;
	::OpenRgn();
// 97-05-11 pkc -- reverse indefinite bar slant to comply with MacOS standard
	::MoveTo(theStamp.left, theStamp.top);
	::LineTo(theStamp.left + theHalfHeight, theStamp.top);
	::LineTo(theStamp.right - 1, theStamp.bottom - 1);
	::LineTo(theStamp.right - (1 + theHalfHeight), theStamp.bottom - 1);
	::LineTo(theStamp.left, theStamp.top);
//	::MoveTo(theStamp.left, theStamp.bottom - 1);
//	::LineTo(theStamp.left + theHalfHeight, theStamp.top);
//	::LineTo(theStamp.right - 1, theStamp.top);
//	::LineTo(theStamp.right - (1 + theHalfHeight), theStamp.bottom - 1);
//	::LineTo(theStamp.left, theStamp.bottom - 1);
	::CloseRgn(theStampRgn);

	GWorldPtr theMacWorld = theMaskWorld.GetMacGWorld();
	if (theMaskWorld.BeginDrawing())
		{
		::EraseRect(&theFrame);
		while (theStamp.left < theFrame.right)
			{
			::FillRgn(theStampRgn, &qd.black);
			::OffsetRgn(theStampRgn,theHeight, 0);
			::OffsetRect(&theStamp, theHeight, 0);
			}		
	
		theMaskWorld.EndDrawing();
		OSErr theErr = ::BitMapToRegion(mFillMask, &((GrafPtr)theMacWorld)->portBits);
		ThrowIfOSErr_(theErr);
		}

	theStamp = theFrame;
	theStamp.right = theStamp.left + RectHeight(theStamp) + theHalfHeight;
		
	if (theMaskWorld.BeginDrawing())
		{
		::EraseRect(&theFrame);
		while (theStamp.left < theFrame.right)
			{
			// 97-05-11 pkc -- reverse indefinite bar slant to comply with MacOS standard
			::MoveTo(theStamp.left, theStamp.top);
			::LineTo(theStamp.left + theHalfHeight, theStamp.top);
			::LineTo(theStamp.right - 1, theStamp.bottom - 1);
//			::MoveTo(theStamp.left, theStamp.bottom - 1);
//			::LineTo(theStamp.left + theHalfHeight, theStamp.top);
//			::LineTo(theStamp.right - 1, theStamp.top);
			::OffsetRect(&theStamp,theHeight, 0);
			}

		theMaskWorld.EndDrawing();
		OSErr theErr = ::BitMapToRegion(mTopBevelMask, &((GrafPtr)theMacWorld)->portBits);
		ThrowIfOSErr_(theErr);
		}

	theStamp = theFrame;
	theStamp.right = theStamp.left + RectHeight(theStamp) + theHalfHeight;
		
	if (theMaskWorld.BeginDrawing())
		{
		::EraseRect(&theFrame);
		while (theStamp.left < theFrame.right)
			{
			// 97-05-11 pkc -- reverse indefinite bar slant to comply with MacOS standard
			::MoveTo(theStamp.right - 1, theStamp.bottom - 1);
			::LineTo(theStamp.right - (1 + theHalfHeight), theStamp.bottom - 1);
			::LineTo(theStamp.left, theStamp.top);
//			::MoveTo(theStamp.right - 1, theStamp.top);
//			::LineTo(theStamp.right - (1 + theHalfHeight), theStamp.bottom - 1);
//			::LineTo(theStamp.left, theStamp.bottom - 1);
			::OffsetRect(&theStamp, theHeight, 0);
			}

		theMaskWorld.EndDrawing();
		OSErr theErr = ::BitMapToRegion(mBottomBevelMask, &((GrafPtr)theMacWorld)->portBits);
		ThrowIfOSErr_(theErr);
		}

	mPatWidth = theHeight;
}



// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//
#pragma mark --- CPatternProgressCaption ---
//
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

CPatternProgressCaption::CPatternProgressCaption(LStream* inStream)
	:	CPatternProgressBar(inStream)
{
	*inStream >> mTraitsID;
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void CPatternProgressCaption::SetDescriptor(ConstStringPtr inDescriptor)
{
	if (inDescriptor == NULL)
		mText = "\p";
	else
		mText = inDescriptor;
	
	if (FocusExposed())
		Draw(NULL);
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void CPatternProgressCaption::SetDescriptor(const char* inCDescriptor)
{
	if (inCDescriptor == NULL)
		mText = "\p";
	else
		mText = inCDescriptor;

	if (FocusExposed())
		Draw(NULL);
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

StringPtr CPatternProgressCaption::GetDescriptor(Str255 outDescriptor) const
{
	return LString::CopyPStr(mText, outDescriptor);
}

// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ
//	₯
// ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ

void CPatternProgressCaption::DrawSelf(void)
{
	CPatternProgressBar::DrawSelf();

	Rect theFrame;
	CalcLocalFrameRect(theFrame);

//	UGraphicGizmos::BevelTintRect(theFrame, -1, 0x4000, 0x6000);

	::InsetRect(&theFrame, 5, 0);
	Int16 theJust = UTextTraits::SetPortTextTraits(mTraitsID);

	if (mText.Length() > 0)
		{
		UGraphicGizmos::PlaceStringInRect(mText, theFrame, theJust);
		}
#ifdef MOZ_OFFLINE
	else if (!UOffline::AreCurrentlyOnline())
		{
		LStr255 offlineString(cOfflineListID, cOfflineStrIndex);
		UGraphicGizmos::PlaceStringInRect(offlineString, theFrame, theJust);
		}
#endif // MOZ_OFFLINE
}

