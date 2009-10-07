#ifndef TMAXPAIRH
#define TMAXPAIRH
//simple template class that keep track of the
//minimum keyed pair
template <class TKey,class TData> class TMaxPair
{
public:
  
    TMaxPair() : m_bLive(false){};
    TKey Key(){return m_Key;};
    TData Data(){return m_Data;};
    bool Valid(){return m_bLive;};
    void Clear(){m_bLive = false;};

    void Update(const TKey & NewKey, const TData & NewData)
    {
        if(NewKey>m_Key || m_bLive==false)
        {
            m_bLive = true;
            m_Key = NewKey;
            m_Data = NewData;
        }
    }

private:
    TKey m_Key;
    TData m_Data;
    bool m_bLive;

};
#endif
