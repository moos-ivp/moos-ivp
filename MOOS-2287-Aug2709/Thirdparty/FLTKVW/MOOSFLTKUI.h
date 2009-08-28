#ifndef MOOSFLTKUIH
#define MOOSFLTKUIH

#include <vector>
class CMOOSFLTKUI : public Fl_Group
{
    public:

    CMOOSFLTKUI( int X, int Y, int W, int H,  const char *l=0 ):Fl_Group(X,Y,W,H)
    {
        m_dfTickPeriod = 1.0;
    };

    ~CMOOSFLTKUI()
    {
        for(unsigned int i = 0;i<CBI.size();i++)
        {
            delete CBI[i];
        }
    }


    Fl_Window * GetRootWindow()
    {
        Fl_Widget * p=this;
        while(1)
        {
            if(p->parent()==NULL)
                return (Fl_Window *)p;
            else
                p = p->parent();
        }
    }

    static bool MOOSDisconnectCallBack(void *pParam)
    {
        CMOOSFLTKUI* pMe = (CMOOSFLTKUI*)pParam;
        return pMe->OnMOOSDisconnect();
    }

    static bool MOOSConnectCallBack(void *pParam)
    {
        CMOOSFLTKUI* pMe = (CMOOSFLTKUI*)pParam;
        return pMe->OnMOOSConnect();
    }
    virtual bool OnMOOSConnect(){return true;};
    virtual bool OnMOOSDisconnect(){return true;};

    struct ControlCallBackInfo
    {
        ControlCallBackInfo(Fl_Widget* pW,void *p,int i){pWidget = pW;pThis = p;ID =i;}
        Fl_Widget* pWidget;
        void * pThis;
        int ID;
    };
    static void OnControlWidgetCallBack(Fl_Widget* pWidget, void * p)
    {
        if(p){
            ControlCallBackInfo* pCCBO  = (ControlCallBackInfo*)p;
            CMOOSFLTKUI* pMe = (CMOOSFLTKUI*)(pCCBO->pThis);
            pMe->OnControlWidget(pWidget,pCCBO->ID);
        }
    }
    void SetID(Fl_Widget* pWidget, int ID)
    {
        ControlCallBackInfo * pCCBO = new ControlCallBackInfo(pWidget,(void*)this,ID);
        pWidget->callback(OnControlWidgetCallBack,pCCBO);
        CBI.push_back(pCCBO);
    }

    void StartTimer(double dfTickPeriod)
    {
        m_dfTickPeriod =dfTickPeriod;
        Fl::add_timeout(m_dfTickPeriod,TimerCallBack,this);
    }
    void StopTimer()
    {
        Fl::remove_timeout(TimerCallBack,this);
    }

    Fl_Widget* GetByID(int nID)
    {
        std::vector<ControlCallBackInfo*>::iterator p;

        for(p = CBI.begin();p!=CBI.end();p++)
        {
            ControlCallBackInfo* pCBI = *p;
            if(pCBI->ID==nID)
            {
                return pCBI->pWidget;
            }
        }
        return NULL;
    }


    static void TimerCallBack(void *p)
    {
        CMOOSFLTKUI* pMe = (CMOOSFLTKUI*)(p);
        pMe->OnTimer();
        Fl::repeat_timeout(pMe->GetTimerPeriod(),TimerCallBack,p);
    }

    double GetTimerPeriod(){return m_dfTickPeriod;};
    virtual void OnTimer(){};
    virtual void OnControlWidget(Fl_Widget* pWidget,int ID){};

private:
    std::vector<ControlCallBackInfo*> CBI;
    double m_dfTickPeriod;


};
#endif
