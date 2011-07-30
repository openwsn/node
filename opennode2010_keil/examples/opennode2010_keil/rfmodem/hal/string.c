//*****************************************************************************
//
// string.c - Routines for drawing text.
//
// Copyright (c) 2007-2008 Luminary Micro, Inc.  All rights reserved.
// 
// Software License Agreement
// 
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's microcontroller products.
// 
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  You may not combine
// this software with "viral" open-source software in order to form a larger
// program.  Any use in violation of the foregoing restrictions may subject
// the user to criminal sanctions under applicable laws, as well as to civil
// liability for the breach of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2523 of the Stellaris Graphics Library.
//
//*****************************************************************************

#include "debug.h"
#include "grlib.h"

//*****************************************************************************
//
//! \addtogroup primitives_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Counts the number of zeros at the start of a word.  This macro uses
// compiler-specific constructs to perform an inline insertion of the "clz"
// instruction, which counts the leading zeros directly.
//
//*****************************************************************************
#if defined(ewarm)
#include <intrinsics.h>
#define NumLeadingZeros(x)      __CLZ(x)
#endif
#if defined(codered) || defined(gcc) || defined(sourcerygxx)
#define NumLeadingZeros(x) __extension__                        \
        ({                                                      \
            register unsigned long __ret, __inp = x;            \
            __asm__("clz %0, %1" : "=r" (__ret) : "r" (__inp)); \
            __ret;                                              \
        })
#endif
#if defined(rvmdk) || defined(__ARMCC_VERSION)
#define NumLeadingZeros(x)      __clz(x)
#endif

//*****************************************************************************
//
//! Determines the width of a string.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param pcString is the string in question.
//! \param lLength is the length of the string.
//!
//! This function determines the width of a string (or portion of the string)
//! when drawn with a particular font.  The \e lLength parameter allows a
//! portion of the string to be examined without having to insert a NULL
//! character at the stopping point (would not be possible if the string was
//! located in flash); specifying a length of -1 will cause the width of the
//! entire string to be computed.
//!
//! \return Returns the width of the string in pixels.
//
//*****************************************************************************
long
GrStringWidthGet(const tContext *pContext, const char *pcString, long lLength)
{
    const unsigned short *pusOffset;
    const unsigned char *pucData;
    long lWidth;

    //
    // Check the arguments.
    //
    ASSERT(pContext);
    ASSERT(pcString);

    //
    // Get some pointers to relevant information in the font to make things
    // easier, and give the compiler a hint about extraneous loads that it can
    // avoid.
    //
    pucData = pContext->pFont->pucData;
    pusOffset = pContext->pFont->pusOffset;

    //
    // Loop through the characters in the string.
    //
    for(lWidth = 0; *pcString && lLength; pcString++, lLength--)
    {
        //
        // Add the width of this character as drawn with the given font.
        //
        lWidth += pucData[pusOffset[*pcString - ' '] + 1];
    }

    //
    // Return the width of the string.
    //
    return(lWidth);
}

//*****************************************************************************
//
//! Draws a string.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param pcString is a pointer to the string to be drawn.
//! \param lLength is the number of characters from the string that should be
//! drawn on the screen.
//! \param lX is the X coordinate of the upper left corner of the string
//! position on the screen.
//! \param lY is the Y coordinate of the upper left corner of the string
//! position on the screen.
//! \param bOpaque is true of the background of each character should be drawn
//! and false if it should not (leaving the background as is).
//!
//! This function draws a string of test on the screen.  The \e lLength
//! parameter allows a portion of the string to be examined without having to
//! insert a NULL character at the stopping point (which would not be possible
//! if the string was located in flash); specifying a length of -1 will cause
//! the entire string to be rendered (subject to clipping).
//!
//! \return None.
//
//*****************************************************************************
void
GrStringDraw(const tContext *pContext, const char *pcString, long lLength,
             long lX, long lY, unsigned long bOpaque)
{
    long lIdx, lX0, lY0, lCount, lOff, lOn, lBit;
    const unsigned char *pucData;
    tContext sCon;

    //
    // Check the arguments.
    //
    ASSERT(pContext);
    ASSERT(pcString);

    //
    // Copy the drawing context into a local structure that can be modified.
    //
    sCon = *pContext;

    //
    // Loop through the characters in the string.
    //
    while(*pcString && lLength--)
    {
        //
        // Stop drawing the string if the right edge of the clipping region has
        // been exceeded.
        //
        if(lX > sCon.sClipRegion.sXMax)
        {
            break;
        }

        //
        // Get a pointer to the font data for the next character from the
        // string.  If there is not a glyph for the next character, replace it
        // with a ".".
        //
        if((*pcString >= ' ') && (*pcString <= '~'))
        {
            pucData = (sCon.pFont->pucData +
                       sCon.pFont->pusOffset[*pcString++ - ' ']);
        }
        else
        {
            pucData = (sCon.pFont->pucData +
                       sCon.pFont->pusOffset['.' - ' ']);
            pcString++;
        }

        //
        // See if the entire character is to the left of the clipping region.
        //
        if((lX + pucData[1]) < sCon.sClipRegion.sXMin)
        {
            //
            // Increment the X coordinate by the width of the character.
            //
            lX += pucData[1];

            //
            // Go to the next character in the string.
            //
            continue;
        }

        //
        // Loop through the bytes in the encoded data for this glyph.
        //
        for(lIdx = 2, lX0 = 0, lBit = 0, lY0 = 0; lIdx < pucData[0]; )
        {
            //
            // See if the bottom of the clipping region has been exceeded.
            //
            if((lY + lY0) > sCon.sClipRegion.sYMax)
            {
                //
                // Stop drawing this character.
                //
                break;
            }

            //
            // See if the font is uncompressed.
            //
            if(sCon.pFont->ucFormat == FONT_FMT_UNCOMPRESSED)
            {
                //
                // Count the number of off pixels from this position in the
                // glyph image.
                //
                for(lOff = 0; lIdx < pucData[0]; )
                {
                    //
                    // Get the number of zero pixels at this position.
                    //
                    lCount = NumLeadingZeros(pucData[lIdx] << (24 + lBit));

                    //
                    // If there were more than 8, then it is a "false" result
                    // since it counted beyond the end of the current byte.
                    // Therefore, simply limit it to the number of pixels
                    // remaining in this byte.
                    //
                    if(lCount > 8)
                    {
                        lCount = 8 - lBit;
                    }

                    //
                    // Increment the number of off pixels.
                    //
                    lOff += lCount;

                    //
                    // Increment the bit position within the byte.
                    //
                    lBit += lCount;

                    //
                    // See if the end of the byte has been reached.
                    //
                    if(lBit == 8)
                    {
                        //
                        // Advance to the next byte and continue counting off
                        // pixels.
                        //
                        lBit = 0;
                        lIdx++;
                    }
                    else
                    {
                        //
                        // Since the end of the byte was not reached, there
                        // must be an on pixel.  Therefore, stop counting off
                        // pixels.
                        //
                        break;
                    }
                }

                //
                // Count the number of on pixels from this position in the
                // glyph image.
                //
                for(lOn = 0; lIdx < pucData[0]; )
                {
                    //
                    // Get the number of one pixels at this location (by
                    // inverting the data and counting the number of zeros).
                    //
                    lCount = NumLeadingZeros(~(pucData[lIdx] << (24 + lBit)));

                    //
                    // If there were more than 8, then it is a "false" result
                    // since it counted beyond the end of the current byte.
                    // Therefore, simply limit it to the number of pixels
                    // remaining in this byte.
                    //
                    if(lCount > 8)
                    {
                        lCount = 8 - lBit;
                    }

                    //
                    // Increment the number of on pixels.
                    //
                    lOn += lCount;

                    //
                    // Increment the bit position within the byte.
                    //
                    lBit += lCount;

                    //
                    // See if the end of the byte has been reached.
                    //
                    if(lBit == 8)
                    {
                        //
                        // Advance to the next byte and continue counting on
                        // pixels.
                        //
                        lBit = 0;
                        lIdx++;
                    }
                    else
                    {
                        //
                        // Since the end of the byte was not reached, there
                        // must be an off pixel.  Therefore, stop counting on
                        // pixels.
                        //
                        break;
                    }
                }
            }

            //
            // Otherwise, the font is compressed with a pixel RLE scheme.
            //
            else
            {
                //
                // See if this is a byte that encodes some on and off pixels.
                //
                if(pucData[lIdx])
                {
                    //
                    // Extract the number of off pixels.
                    //
                    lOff = (pucData[lIdx] >> 4) & 15;

                    //
                    // Extract the number of on pixels.
                    //
                    lOn = pucData[lIdx] & 15;

                    //
                    // Skip past this encoded byte.
                    //
                    lIdx++;
                }

                //
                // Otherwise, see if this is a repeated on pixel byte.
                //
                else if(pucData[lIdx + 1] & 0x80)
                {
                    //
                    // There are no off pixels in this encoding.
                    //
                    lOff = 0;

                    //
                    // Extract the number of on pixels.
                    //
                    lOn = (pucData[lIdx + 1] & 0x7f) * 8;

                    //
                    // Skip past these two encoded bytes.
                    //
                    lIdx += 2;
                }

                //
                // Otherwise, this is a repeated off pixel byte.
                //
                else
                {
                    //
                    // Extract the number of off pixels.
                    //
                    lOff = pucData[lIdx + 1] * 8;

                    //
                    // There are no on pixels in this encoding.
                    //
                    lOn = 0;

                    //
                    // Skip past these two encoded bytes.
                    //
                    lIdx += 2;
                }
            }

            //
            // Loop while there are any off pixels.
            //
            while(lOff)
            {
                //
                // See if the bottom of the clipping region has been exceeded.
                //
                if((lY + lY0) > sCon.sClipRegion.sYMax)
                {
                    //
                    // Ignore the remainder of the on pixels.
                    //
                    break;
                }

                //
                // See if there is more than one on pixel that will fit onto
                // the current row.
                //
                if((lOff > 1) && ((lX0 + 1) < pucData[1]))
                {
                    //
                    // Determine the number of on pixels that will fit on this
                    // row.
                    //
                    lCount = (((lX0 + lOff) > pucData[1]) ? pucData[1] - lX0 :
                              lOff);

                    //
                    // If this row is within the clipping region, draw a
                    // horizontal line that corresponds to the sequence of on
                    // pixels.
                    //
                    if(((lY + lY0) >= sCon.sClipRegion.sYMin) && bOpaque)
                    {
                        sCon.ulForeground = pContext->ulBackground;
                        GrLineDrawH(&sCon, lX + lX0, lX + lX0 + lCount - 1,
                                    lY + lY0);
                    }

                    //
                    // Decrement the count of on pixels by the number on this
                    // row.
                    //
                    lOff -= lCount;

                    //
                    // Increment the X offset by the number of on pixels.
                    //
                    lX0 += lCount;
                }

                //
                // Otherwise, there is only a single on pixel that can be
                // drawn.
                //
                else
                {
                    //
                    // If this pixel is within the clipping region, then draw
                    // it.
                    //
                    if(((lX + lX0) >= sCon.sClipRegion.sXMin) &&
                       ((lX + lX0) <= sCon.sClipRegion.sXMax) &&
                       ((lY + lY0) >= sCon.sClipRegion.sYMin) && bOpaque)
                    {
                        DpyPixelDraw(pContext->pDisplay, lX + lX0, lY + lY0,
                                     pContext->ulBackground);
                    }

                    //
                    // Decrement the count of on pixels.
                    //
                    lOff--;

                    //
                    // Increment the X offset.
                    //
                    lX0++;
                }

                //
                // See if the X offset has reached the right side of the
                // character glyph.
                //
                if(lX0 == pucData[1])
                {
                    //
                    // Increment the Y offset.
                    //
                    lY0++;

                    //
                    // Reset the X offset to the left side of the character
                    // glyph.
                    //
                    lX0 = 0;
                }
            }

            //
            // Loop while there are any on pixels.
            //
            while(lOn)
            {
                //
                // See if the bottom of the clipping region has been exceeded.
                //
                if((lY + lY0) > sCon.sClipRegion.sYMax)
                {
                    //
                    // Ignore the remainder of the on pixels.
                    //
                    break;
                }

                //
                // See if there is more than one on pixel that will fit onto
                // the current row.
                //
                if((lOn > 1) && ((lX0 + 1) < pucData[1]))
                {
                    //
                    // Determine the number of on pixels that will fit on this
                    // row.
                    //
                    lCount = (((lX0 + lOn) > pucData[1]) ? pucData[1] - lX0 :
                              lOn);

                    //
                    // If this row is within the clipping region, draw a
                    // horizontal line that corresponds to the sequence of on
                    // pixels.
                    //
                    if((lY + lY0) >= sCon.sClipRegion.sYMin)
                    {
                        sCon.ulForeground = pContext->ulForeground;
                        GrLineDrawH(&sCon, lX + lX0, lX + lX0 + lCount - 1,
                                    lY + lY0);
                    }

                    //
                    // Decrement the count of on pixels by the number on this
                    // row.
                    //
                    lOn -= lCount;

                    //
                    // Increment the X offset by the number of on pixels.
                    //
                    lX0 += lCount;
                }

                //
                // Otherwise, there is only a single on pixel that can be
                // drawn.
                //
                else
                {
                    //
                    // If this pixel is within the clipping region, then draw
                    // it.
                    //
                    if(((lX + lX0) >= sCon.sClipRegion.sXMin) &&
                       ((lX + lX0) <= sCon.sClipRegion.sXMax) &&
                       ((lY + lY0) >= sCon.sClipRegion.sYMin))
                    {
                        DpyPixelDraw(pContext->pDisplay, lX + lX0, lY + lY0,
                                     pContext->ulForeground);
                    }

                    //
                    // Decrement the count of on pixels.
                    //
                    lOn--;

                    //
                    // Increment the X offset.
                    //
                    lX0++;
                }

                //
                // See if the X offset has reached the right side of the
                // character glyph.
                //
                if(lX0 == pucData[1])
                {
                    //
                    // Increment the Y offset.
                    //
                    lY0++;

                    //
                    // Reset the X offset to the left side of the character
                    // glyph.
                    //
                    lX0 = 0;
                }
            }
        }

        //
        // Increment the X coordinate by the width of the character.
        //
        lX += pucData[1];
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
