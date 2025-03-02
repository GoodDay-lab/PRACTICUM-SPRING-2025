#include <iostream>
#include <cmath>

// Start: 00:00:00 01.01.2000

typedef _Float64 f64;
typedef __uint32_t u32;
typedef __int64_t i64;
typedef __int32_t i32;

static u32 MonthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


class DateTimeSpan {
    private:
        f64 time;
    public:
        DateTimeSpan(const DateTimeSpan& d) : time(d.time) {};
        DateTimeSpan(f64 d = 0.0) : time(d) {};
        DateTimeSpan(i32 d = 0, i32 h = 0, i32 m = 0, i32 s = 0);
    
        f64 getReal() const {
            return time;
        }
    
        void getTime(i32 *To) const;
    
        void print_out() const {
            i32 t[4];
    
            getTime(t);
            printf("%d days %0.2d:%0.2d:%0.2d\n", t[0], t[1], t[2], t[3]);
        }

        DateTimeSpan& operator- () {
            time = -time;
            return *this;
        }

        DateTimeSpan operator+ (const DateTimeSpan& s) {
            return DateTimeSpan(time + s.time);
        }

        DateTimeSpan operator- (const DateTimeSpan& s) {
            return DateTimeSpan(time - s.time);
        }
    };

class DateTime {
private:
    f64 time;
public:
    DateTime(u32 d = 1, u32 mo = 1, u32 y = 2000, u32 h = 0, u32 mi = 0, u32 s = 0);
    DateTime(f64 d = 0.0) : time(d) {};

    f64 getReal() const {
        return time;
    }

    void getTime(u32 *To) const;
    void getDate(u32 *To) const;

    u32 getMonthDays(u32 m, u32 y) const {
        if ((y % 4 == 0) && (m == 1))
            return 29;
        return MonthDays[m];
    }

    u32 getYearDays(u32 y) const {
        if (y % 4 == 0)
            return 366;
        return 365;
    }

    void print_out() const {
        u32 t[3], d[3];

        getTime(t); getDate(d);
        printf("%0.2d:%0.2d:%0.2d %0.2d/%0.2d/%0.4d\n", t[0], t[1], t[2], d[0], d[1], d[2]);
    }

    friend DateTimeSpan operator- (const DateTime& d1, const DateTime& d2) {
        return DateTimeSpan(d1.getReal() - d2.getReal());
    }

    DateTime& operator+ (const DateTimeSpan& s1) {
        time += s1.getReal();
        return *this;
    }

    DateTime& operator- (const DateTimeSpan& s1) {
        time += s1.getReal();
        return *this;
    }
};

DateTime::DateTime(u32 d, u32 mo, u32 y, u32 h, u32 mi, u32 s)
{
    time = 0.0;

    while (y != 2000) {
        if (y > 2000) {
            y -= 1; time += getYearDays(y);
        } else {
            y += 1; time -= getYearDays(y-1);
        }
    }

    while (mo != 1) {
        mo -= 1; time += getMonthDays(mo, y);
    }

    while (d != 1) {
        d -= 1; time += 1;
    }

    while (h != 0) {
        h -= 1; time += (f64)1.0/24;
    }

    while (mi != 0) {
        mi -= 1; time += (f64)1.0/(24*60);
    }

    while (s != 0) {
        s -= 1; time += (f64)1.0/(24*3600);
    }
}

void DateTime::getTime(u32 *To) const {
    f64 time_part = time - floor(time);
    u32 time_int = time_part*(3600*24);
    
    To[0] = time_int / 3600;
    To[1] = (time_int / 60) % 60;
    To[2] = time_int % 60;
}

void DateTime::getDate(u32* To) const {
    i64 date_part = floor(time);
    u32 d = 0, m = 0, y = 2000;

    while (date_part < 0) {
        y -= 1; date_part += getYearDays(y);
    }

    while (date_part != 0) {
        if (date_part >= getYearDays(y)) {
            date_part -= getYearDays(y); y += 1;
        } else if (date_part >= getMonthDays(m, y)) {
            date_part -= getMonthDays(m, y); m += 1;
        } else {
            date_part -= 1; d += 1;
        }
    }

    To[0] = d+1;
    To[1] = m+1;
    To[2] = y;
}

DateTimeSpan::DateTimeSpan(i32 d, i32 h, i32 m, i32 s) {
    time = d;
    time += (f64)h / 24;
    time += (f64)m / (24 * 60);
    time += (f64)s / (24 * 3600);
}

void DateTimeSpan::getTime(i32* To) const {
    To[0] = trunc(time);

    f64 part = time - To[0];
    i32 time_int = std::abs(part)*(3600*24);
    
    To[1] = time_int / 3600;
    To[2] = (time_int / 60) % 60;
    To[3] = time_int % 60;
}


int main() {
    DateTime dl1 (1, 2, 2006, 0, 0, 0);
    DateTime dl2 (28, 2, 2001, 10, 15, 54);
    DateTimeSpan dt1(1, -10, 3, 52);
    DateTimeSpan dt2(3, 4, 2, 3);
    DateTimeSpan dt3 = dt2 + dt1;
    DateTimeSpan dt4 = dt2 - dt1;
    DateTimeSpan dt5 = dl2 - dl1;
    DateTime dl3 (8, 3, 2024, 0, 0, 0);
    DateTime dl4 (426.0);
    DateTime dl5 (2, 3, 2001, 10, 15, 54);

    std::cout << (double)dl3.getReal() << std::endl;
    std::cout << (double)dl2.getReal() << std::endl;
    std::cout << (double)dl4.getReal() << std::endl;
    std::cout << (double)dl5.getReal() << std::endl;

    dt1.print_out();
    dt2.print_out();
    dt3.print_out();
    dt4.print_out();
    dt5.print_out();
    dl2.print_out();
    dl3.print_out();
    dl4.print_out();
    dl5.print_out();
}
