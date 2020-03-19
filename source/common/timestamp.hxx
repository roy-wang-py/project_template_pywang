#ifndef __SQHAO_TIMESTAMP_HXX__
#define __SQHAO_TIMESTAMP_HXX__

#include <utility>
#include <ctime>
#include <limits>
#include <time.h>
#include <unistd.h>
#include <algorithm>
#include "timespan.hxx"
#include "Exception.hxx"

NAMESPACE_PREFIX

class  Timestamp
	/// Timestamps are UTC (Coordinated Universal Time)
	/// based and thus independent of the timezone
	/// in effect on the system.
	///
	/// The internal reference time is the Unix epoch, 
	/// midnight, January 1, 1970.
{
	typedef long long int Int64;
public:
	typedef Int64 TimeVal; 
		/// Monotonic UTC time value in microsecond resolution,
		/// with base time midnight, January 1, 1970.
		
	typedef Int64 UtcTimeVal; 
		/// Monotonic UTC time value in 100 nanosecond resolution,
		/// with base time midnight, October 15, 1582.
		
	typedef Int64 TimeDiff;
		/// Difference between two TimeVal values in microseconds.

	static const TimeVal TIMEVAL_MIN =std::numeric_limits<Timestamp::TimeVal>::min();
	static const TimeVal TIMEVAL_MAX = std::numeric_limits<Timestamp::TimeVal>::max();

	Timestamp(){
		update();
	}	
	Timestamp(TimeVal tv):_ts(tv){}
	Timestamp(const Timestamp& other):_ts(other._ts){}
	~Timestamp()=default;
	Timestamp& operator = (const Timestamp& other){
		_ts = other._ts;
		return *this;
	}
	Timestamp& operator = (TimeVal tv){
		_ts = tv;
		return *this;
	}
	void swap(Timestamp& timestamp){
		std::swap(_ts, timestamp._ts);
	}		
	void update(){
		struct timespec ts;
		if (clock_gettime(CLOCK_REALTIME, &ts))
			throw SystemException("cannot get time of day");
		_ts = TimeVal(ts.tv_sec)*resolution() + ts.tv_nsec/1000;
	}
	bool operator == (const Timestamp& ts) const{
		return _ts == ts._ts;
	}
	bool operator != (const Timestamp& ts) const{
		return _ts != ts._ts;
	}
	bool operator >  (const Timestamp& ts) const{
		return _ts > ts._ts;
	}
	bool operator >= (const Timestamp& ts) const{
		return _ts >= ts._ts;
	}
	bool operator <  (const Timestamp& ts) const{
		return _ts < ts._ts;
	}
	bool operator <= (const Timestamp& ts) const{
		return _ts <= ts._ts;
	}
	Timestamp operator + (Timestamp::TimeDiff d) const{
		return Timestamp(_ts + d);
	}
	Timestamp operator - (Timestamp::TimeDiff d) const{
		return Timestamp(_ts - d);
	}
	Timestamp::TimeDiff operator - (const Timestamp& ts) const{
		return _ts - ts._ts;
	}
	Timestamp& operator += (Timestamp::TimeDiff d){
		_ts += d;
		return *this;
	}
	Timestamp& operator -= (Timestamp::TimeDiff d){
		_ts -= d;
		return *this;
	}
	Timestamp  operator +  (const Timespan& span) const{
		return Timestamp(_ts + span.totalMicroseconds());
	}

	Timestamp  operator -  (const Timespan& span) const{
		return Timestamp(_ts - span.totalMicroseconds());
	}

	Timestamp&  operator += (const Timespan& span){
		_ts += span.totalMicroseconds();
		return *this;
	}

	Timestamp& operator -= (const Timespan& span){
		_ts -= span.totalMicroseconds();
		return *this;
	}

	static Timestamp fromEpochTime(std::time_t t){
		return Timestamp(TimeVal(t)*resolution());
	}
		
	static Timestamp fromUtcTime(UtcTimeVal val){
		val -= (TimeDiff(0x01b21dd2) << 32) + 0x13814000;
		val /= 10;
		return Timestamp(val);
	}
		
	std::time_t epochTime() const{
		return std::time_t(_ts/resolution());
	}
	UtcTimeVal utcTime() const{
		return _ts*10 + (TimeDiff(0x01b21dd2) << 32) + 0x13814000;
	}
	TimeVal epochMicroseconds() const{
		return _ts;
	}
	TimeDiff elapsed() const{
		Timestamp now;
		return now - *this;
	}
	bool isElapsed(Timestamp::TimeDiff interval) const{
		Timestamp now;
		Timestamp::TimeDiff diff = now - *this;
		return diff >= interval;
	}
	void swap(Timestamp& s1, Timestamp& s2){
		s1.swap(s2);
	}
	TimeVal raw() const{
		return _ts;
	}
	static TimeDiff resolution(){
		return 1000000;
	}
private:
	TimeVal _ts;
};


NAMESPACE_SUBFIX


#endif // __SQHAO_TIMESTAMP_HXX__
