#pragma once
/* 
 * File:   CHoldInterrupts.h
 * Author: C. Arena
 *
 * Created on March 26, 2016, 11:07 AM
 */

class CHoldInterrupts
{
    CHoldInterrupts(const CHoldInterrupts& orig);     // private to prevent use
    CHoldInterrupts & operator=(const CHoldInterrupts& rhs);

public:
    CHoldInterrupts();
    ~CHoldInterrupts();

    static bool getPriMask(void);

private:
    uint32_t mPriMask;
};

