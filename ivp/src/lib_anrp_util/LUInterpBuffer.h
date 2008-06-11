#ifndef LUINTERP_BUFFER_H
#define LIINTERP_BUFFER_H

#include <map>
#include <iterator>
#include <iostream>
#include <assert.h>

// a buffer to store data and get interp values by index with time

template <class Key, class Data, class InterpFunc, class Compare = std::less<Key> >
class LUInterpBuffer : public std::map<Key, Data, Compare>
{
		InterpFunc m_linearinterp;

	public:
		typedef typename std::map<Key, Data, Compare>::iterator iterator;
		void SetInterpFunc(const InterpFunc &interp)
		{
			m_linearinterp = interp;
		}

		InterpFunc &GetInterpFunc()
		{
			return m_linearinterp;
		}

		Data operator()(const Key &interp_time)
		{
			assert(this->size() > 0 );

			if (this->size() == 1)
				return this->begin()->second;

			iterator hi, low;

			hi = this->lower_bound(interp_time);

			low = hi;

			advance(low, -1);

			if (hi != this->begin() && hi != this->end()) {
				return m_linearinterp(*low, *hi, interp_time);
			} else {
				// interp time not in range
				double a = interp_time, b = this->begin()->first, c = this->rbegin()->first;
				std::cerr << "Warning!!!! interp time " << (a - b)
				<< " is not in range [" << (b - b) << ", " << (c - b) << "]" << std::endl;

				if (hi == this->begin()) {
					iterator hi2 = hi;
					hi2++;
					return m_linearinterp(*hi, *hi2, interp_time);
				} else if (hi == this->end()) {
					iterator low2 = low;
					advance(low2, -1);
					return m_linearinterp(*low2, *low, interp_time);
				}
			}

			assert("should not get here" == 0);
			return Data();
		}

		void EraseOld(double dfTime)
		{
			iterator oldest = lower_bound((Key)dfTime);
			erase(this->begin(), oldest);
		}

		Key MaxKey() const
		{
			Key maxkey;

			if (this->size()) {
				maxkey = this->rbegin()->first;
			}

			return maxkey;
		}
};

#endif
