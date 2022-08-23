#ifndef __HGRID__HH
#define __HGRID__HH

/*
 *	HwxGrid is wxGrid that would fit to parent width by resizing its columns 
 *		according to their stretch factors, assigned by user;
 *
 *	Stretch Factor may be:
 *			0: 		for AutoSize columns, where size is set by wxGrid to fit content 
 *			negative:       for fixed size columns where size is abs(stretch factor). 
 *			positive:	the more stretch factor the more space column would get on resize
 *
 *	Example: to get grid with one autosized column, one fixed(50px) column and two more stretchable
 		columns we SetColStretch(0,0); 
			   SetColStretch(1,-50); 
			   SetColStretch(2,1);
			   SetColStretch(3,1);
 *
 *	(c) Yuri Borsky		brahmbeus@gmail.com
 *		 
 *	Usual wx license terms applies
 */

#include <wx/grid.h>		 

#define HwxGrid_MAXF	(1024)		 
		 		 
class HwxGrid: public wxGrid
{
	int sf[HwxGrid_MAXF];
	
	void OnSizeEvt( wxSizeEvent& ev )
	{
		if( !StretchIt() ) ev.Skip();		
	}
	int StretchIt()
	{
		int new_width = GetClientSize().GetWidth()-GetRowLabelSize()-10;
		int fixedWidth=0, numStretches=0, numStretched=0;
		
		for( int i=0; i<GetNumberCols(); ++i )
		{
			if( sf[i] == 0 ) fixedWidth += GetColSize(i);
			else if( sf[i] < 0 ) {
				AutoSizeColumn(i,false);
				fixedWidth += GetColSize(i);
			} else {
				numStretches += sf[i];
				numStretched += 1;
			}
		}
		
		// Now either we have space for normal layout or resort to wxGrid default behaviour	
		if( numStretched && ((fixedWidth + numStretched*10) < new_width) )
		{
			int stretchSpace = (new_width - fixedWidth)/numStretches;
			//BeginBatch();				
			for( unsigned i=0; i<GetNumberCols(); ++i )
				if( sf[i] > 0 )
					SetColSize(i,stretchSpace*sf[i]);
			//EndBatch();
			return 1;
		} 
		return 0;
	}

	int keepFit;
		
   public:
	HwxGrid( wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxWANTS_CHARS,
	    const wxString& name = wxGridNameStr )
		   : wxGrid(parent,id,pos,size,style,name), keepFit(1)
   	{
		// Initially all columns have s-factor=1
		for( unsigned i=0; i<HwxGrid_MAXF; ++i ) sf[i]=1; 
		
		Connect( wxEVT_SIZE, wxSizeEventHandler( HwxGrid::OnSizeEvt) );
	} 
	virtual ~HwxGrid() {}
	
	void SetColStretch ( unsigned i, int factor ) {	if( i < HwxGrid_MAXF ) sf[i]=factor; }
	int  GetColStretch ( unsigned i ) const { return (i<HwxGrid_MAXF)?sf[i]:1; }
	void ReLayout() { StretchIt(); }
	void SetFit( int fit_style ) { keepFit=fit_style; }
};  

#endif
