// QSlotLayout.h: where every part of the quick-slot bar goes.
//
// The bar is laid out in the 1920x1080 pixels the artwork was drawn at, then
// scaled by UIScale().

#if !defined(QSLOTLAYOUT_H__4D7C0A19_5E63_41B8_8C2F_7A9E10D4B336__INCLUDED_)
#define QSLOTLAYOUT_H__4D7C0A19_5E63_41B8_8C2F_7A9E10D4B336__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// mnQSlot, one row of the bar.
#define QSLOT_BAR_WIDTH					336
#define QSLOT_BAR_HEIGHT				40
#define QSLOT_BAR_OFFSET_Y				4
#define QSLOT_ROW_PITCH					42

// The icons sitting on it.  Every icon in item.tex is 28x28.
#define QSLOT_ICON_ORIGIN_X				8
#define QSLOT_ICON_ORIGIN_Y				11
#define QSLOT_ICON_EXTENT				28
#define QSLOT_COLUMN_PITCH				31

// diskill is 30x30 - a one pixel border drawn around an icon.
#define QSLOT_DISABLE_BORDER			1

// mnQSlotN, the strip of key numbers under the bar.
#define QSLOT_KEYSTRIP_OFFSET_X			(-5)
#define QSLOT_KEYSTRIP_OFFSET_Y			0

// mnQup / mnQdn, the tab button at the right hand end, and the tab number
// drawn just under it.
#define QSLOT_TAB_BTN_OFFSET_X			325
#define QSLOT_TAB_BTN_OFFSET_Y			12
#define QSLOT_TAB_BTN_EXTENT			8
#define QSLOT_TAB_TEXT_OFFSET_X			1
#define QSLOT_TAB_TEXT_OFFSET_Y			8

// The item count sits across the top edge of its icon.
#define QSLOT_COUNT_OFFSET_Y			(-3)

struct QSLOT_RECT
{
	int x, y, cx, cy;
};

// Where the bar is and how much bigger than authored size it is drawn.
struct QSLOT_LAYOUT
{
	int		nOriginX;
	int		nOriginY;
	float	fScale;
};

// One place rounds, so a rectangle built here twice is the same rectangle.
inline int QSlotScale(const QSLOT_LAYOUT& layout, float fValue)
{
	const float fScaled = fValue * layout.fScale;
	return (int)(fScaled < 0.0f ? fScaled - 0.5f : fScaled + 0.5f);
}

inline QSLOT_RECT QSlotBarRect(const QSLOT_LAYOUT& layout, int nRow)
{
	QSLOT_RECT rc;
	rc.x  = layout.nOriginX;
	rc.y  = layout.nOriginY + QSlotScale(layout, (float)(QSLOT_BAR_OFFSET_Y - nRow * QSLOT_ROW_PITCH));
	rc.cx = QSlotScale(layout, (float)QSLOT_BAR_WIDTH);
	rc.cy = QSlotScale(layout, (float)QSLOT_BAR_HEIGHT);
	return rc;
}

inline QSLOT_RECT QSlotIconRect(const QSLOT_LAYOUT& layout, int nColumn, int nRow)
{
	QSLOT_RECT rc;
	rc.x  = layout.nOriginX + QSlotScale(layout, (float)(QSLOT_ICON_ORIGIN_X + nColumn * QSLOT_COLUMN_PITCH));
	rc.y  = layout.nOriginY + QSlotScale(layout, (float)(QSLOT_ICON_ORIGIN_Y - nRow * QSLOT_ROW_PITCH));
	rc.cx = QSlotScale(layout, (float)QSLOT_ICON_EXTENT);
	rc.cy = rc.cx;
	return rc;
}

// The red "cannot use this skill" overlay, one authored pixel outside the icon.
inline QSLOT_RECT QSlotDisableRect(const QSLOT_LAYOUT& layout, int nColumn, int nRow)
{
	QSLOT_RECT rc;
	rc.x  = layout.nOriginX + QSlotScale(layout, (float)(QSLOT_ICON_ORIGIN_X - QSLOT_DISABLE_BORDER + nColumn * QSLOT_COLUMN_PITCH));
	rc.y  = layout.nOriginY + QSlotScale(layout, (float)(QSLOT_ICON_ORIGIN_Y - QSLOT_DISABLE_BORDER - nRow * QSLOT_ROW_PITCH));
	rc.cx = QSlotScale(layout, (float)(QSLOT_ICON_EXTENT + 2 * QSLOT_DISABLE_BORDER));
	rc.cy = rc.cx;
	return rc;
}

inline QSLOT_RECT QSlotKeyStripRect(const QSLOT_LAYOUT& layout)
{
	QSLOT_RECT rc;
	rc.x  = layout.nOriginX + QSlotScale(layout, (float)QSLOT_KEYSTRIP_OFFSET_X);
	rc.y  = layout.nOriginY + QSlotScale(layout, (float)QSLOT_KEYSTRIP_OFFSET_Y);
	rc.cx = 0;
	rc.cy = 0;
	return rc;
}

inline QSLOT_RECT QSlotTabButtonRect(const QSLOT_LAYOUT& layout)
{
	QSLOT_RECT rc;
	rc.x  = layout.nOriginX + QSlotScale(layout, (float)QSLOT_TAB_BTN_OFFSET_X);
	rc.y  = layout.nOriginY + QSlotScale(layout, (float)QSLOT_TAB_BTN_OFFSET_Y);
	rc.cx = QSlotScale(layout, (float)QSLOT_TAB_BTN_EXTENT);
	rc.cy = rc.cx;
	return rc;
}

// The item count is right aligned on its icon; nTextWidth is what the font
// reports, already in real pixels, so it must not be scaled again.
inline void QSlotCountTextPos(const QSLOT_LAYOUT& layout, int nColumn, int nRow,
							  int nTextWidth, int* o_pnX, int* o_pnY)
{
	const QSLOT_RECT rcIcon = QSlotIconRect(layout, nColumn, nRow);
	*o_pnX = rcIcon.x + rcIcon.cx - nTextWidth;
	*o_pnY = rcIcon.y + QSlotScale(layout, (float)QSLOT_COUNT_OFFSET_Y);
}

// Cooldowns and the like are centred on their icon.
inline void QSlotCenterTextPos(const QSLOT_LAYOUT& layout, int nColumn, int nRow,
							   int nTextWidth, int nTextHeight, int* o_pnX, int* o_pnY)
{
	const QSLOT_RECT rcIcon = QSlotIconRect(layout, nColumn, nRow);
	*o_pnX = rcIcon.x + (rcIcon.cx - nTextWidth) / 2;
	*o_pnY = rcIcon.y + (rcIcon.cy - nTextHeight) / 2;
}

inline bool QSlotPointInRect(const QSLOT_RECT& rc, int x, int y)
{
	return (x >= rc.x) && (x < rc.x + rc.cx) && (y >= rc.y) && (y < rc.y + rc.cy);
}

// Which slot the cursor is over, if any.  nRowCount is how many rows the bar is
// currently showing.
inline bool QSlotHitTest(const QSLOT_LAYOUT& layout, int nColumnCount, int nRowCount,
						 int x, int y, int* o_pnColumn, int* o_pnRow)
{
	for(int nRow = 0; nRow < nRowCount; nRow++)
	{
		for(int nColumn = 0; nColumn < nColumnCount; nColumn++)
		{
			if(QSlotPointInRect(QSlotIconRect(layout, nColumn, nRow), x, y))
			{
				if(o_pnColumn)	*o_pnColumn = nColumn;
				if(o_pnRow)		*o_pnRow    = nRow;
				return true;
			}
		}
	}
	return false;
}

#endif // !defined(QSLOTLAYOUT_H__4D7C0A19_5E63_41B8_8C2F_7A9E10D4B336__INCLUDED_)
