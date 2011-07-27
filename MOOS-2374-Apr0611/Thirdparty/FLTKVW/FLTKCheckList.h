#ifndef CFLTKCheckListH
#define CFLTKCheckListH
#include "Flv_Table.H"
#include <FL/fl_draw.H>

#include <map>
#include <vector>
#include <string>
#include <iostream>


class CFLTKCheckList :  public Flv_Table
{
private:
    typedef Flv_Table BASE;
public:
    CFLTKCheckList( int X, int Y, int W, int H, const char *l ) : BASE(X,Y,W,H,l)
    {
        cols(2);
        //set up callbacks
        callback_when( FLVEcb_CLICKED );
        callback((Fl_Callback*)ListCallback);
        max_clicks(2);
        Flv_Style s;
        BASE::get_style(s,0,0);
        global_style.background(FL_GRAY);
        color(FL_BLUE);

        has_scrollbar(FLVS_VERTICAL);

        feature(FLVF_DIVIDERS|FLVF_MULTI_SELECT|FLVF_FULL_RESIZE);
        feature_remove(FLVF_MULTI_SELECT | FLVF_COL_HEADER |FLVF_DIVIDERS);
        AddItem("lala",true);
        AddItem("la",false);
        AddItem("spock",true);
        AddItem("bock",true);

    }

    void OnListCallBack()
    {
        if(why_event()==FLVE_CLICKED)
        {
            //this is a double click
            int c = select_start_col();
            int r = select_start_row();
            m_Data[r].second = !m_Data[r].second;
        }
    }

    static void ListCallback(CFLTKCheckList *pMe, void * )
    {
        pMe->OnListCallBack();
        std::cout<<"InCallBack"<<std::endl;
    }
    
    virtual void add_selection_style( Flv_Style &s, int R, int C ){};


    void AddItem(const std::string & sStr,bool bInitialValue)
    {
        m_Data.push_back( std::pair<std::string, bool>(sStr,bInitialValue));
        m_Index[sStr] = m_Data.size()-1;      
        rows(m_Data.size());
    }

    bool IsChecked(const std::string & sStr)
    {
        std::map<std::string, int>::iterator p = m_Index.find(sStr);

        if(p!=m_Index.end())
        {
            return m_Data[p->second].second;
        }
        else
        {
            return false;
        }
    }


    virtual int col_width( int C )
    {
        static int LW=-1;
        static int cw[2];

        int scrollbar_width = (scrollbar.visible()?scrollbar.w():0);
        int W = w()- scrollbar_width-1;
    
        //  Either always calculate or be sure to check that width
        //  hasn't changed.
        if (W!=LW)                          //  Width change, recalculate
        {
            cw[0] = (W*80)/100;
            cw[1] = (W-cw[0]-1);
        }
        return cw[C];

    }

    virtual void draw_cell( int Offset, int &X, int &Y, int &W, int &H, int R, int C )
    {
        Flv_Style s;
        get_style(s, R, C);
        Flv_Table::draw_cell(Offset,X,Y,W,H,R,C);

        if(R>-1 && R<(int)m_Data.size())
        {
            if(C==0)
            {
                fl_draw(m_Data[R].first.c_str(), X-Offset, Y, W, H, s.align() );
            }
            else if(C==1)
            {
                if(m_Data[R].second)
                {
                    #define SZ 5
                    fl_draw_symbol("@square", X-Offset+(W-SZ)/2, Y+(H-SZ)/2, SZ,
                                            SZ, (Fl_Color)(FL_RED) );
                }
                else
                {
                    fl_draw_symbol("@square", X-Offset+(W-SZ)/2, Y+(H-SZ)/2, SZ,
                                            SZ, (Fl_Color)(FL_LIGHT1) );
                }
            }
        }
    };
    
    
    private:
    std::vector< std::pair<std::string,bool> > m_Data;
    std::map<std::string, int> m_Index; 
    

};

#endif
