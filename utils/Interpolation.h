/** @file Interpolation.h
 *
 * Copyright (c) 2020 IACE
 */
#ifndef INTERPOLATION_H
#define INTERPOLATION_H

struct Vec {
    double x, y;
};

/**
 * Base class for an interpolator
 */
class Interpolator {
public:
    /**
     * Constructor for a general interpolator
     * @param count number of points for interpolator
     */
    Interpolator(const unsigned int count) :
            iCount(count), P(new Vec[count]()) {}

    /**
     * Reinitialise interpolator with new dataset
     * @param dx array of x values
     * @param dy array of y values
     * @param iCount of values in array
     * @param bSort true if arrays must be sorted
     */
    void setData(double *dx, double *dy, unsigned int iCount, bool bSort=false) {
        this->iCount = iCount;
        delete[] this->P;
        this->P = new Vec[this->iCount];

        this->updateData(dx, dy, bSort);
    }

    /**
     * update the points in dataset
     * @param dx array of x values
     * @param dy array of y values
     * @param bSort true if arrays must be sorted
     */
    void updateData(double *dx, double *dy, bool bSort = false) {
        if (!dx || !dy)
            return;

        for (unsigned int i = 0; i < this->iCount; ++i) {
            this->P[i].x = dx[i];
            this->P[i].y = dy[i];
        }

        if (bSort)
            sort();
    }

    /**
     * virtual destructor
     */
    virtual ~Interpolator() {
        delete[] this->P;
    }

    /**
     * Operator overload to get interpolated value.
     * @param dx x position to interpolate
     */
    double operator()(double dx) {
        if (this->P)
            return this->interpolate(dx);
        else
            return 0;
    }

protected:
    ///\cond false
    virtual double interpolate(double dx) = 0;

    unsigned int iCount;                 ///< number of points in array
    Vec *P = nullptr;                              ///< points with x and y values

private:
    void sort() {
        Vec vTemp;
        for(unsigned int i = 0; i < iCount - 1; i++) {
            for(unsigned int j = 0; j < iCount - i - 1; j++) {
                if (this->P[j].x > this->P[j + 1].x){
                    vTemp = this->P[j];
                    this->P[j] = this->P[j + 1];
                    this->P[j + 1] = vTemp;
                }
            }
        }
    }
    ///\endcond
};

/**
 * Class implementing linear interpolation
 * for a fixed number of datapoints
 */
class LinearInterpolator : public Interpolator {
public:
    /**
     * Constructor for a linear interpolator
     * @param dx array of x values
     * @param dy array of y values
     * @param iCount of values in array
     * @param bSort true if arrays must be sorted
     */
    LinearInterpolator(double *dx, double *dy, const unsigned int iCount, bool bSort=false)
            : Interpolator(iCount) {
        this->updateData(dx, dy, bSort);
    }

    LinearInterpolator(const unsigned int count) : Interpolator(count) {}

    LinearInterpolator() : Interpolator(1) {}

protected:
    ///\cond false
    double interpolate(double dx) {
        if (dx <= this->P[0].x) {
            return this->P[0].y;
        }
        if (dx >= this->P[this->iCount - 1].x) {
            return this->P[iCount - 1].y;
        }

        double dValue = 0;
        for (unsigned int i = 0; i < iCount - 1; ++i) {
            if (dx > this->P[i + 1].x) {
                continue;
            }
            double dm = ((this->P[i + 1].y - this->P[i].y) / (this->P[i + 1].x - this->P[i].x));

            dValue = dm * (dx - this->P[i].x) + this->P[i].y;
            break;
        }
        return dValue;
    }
    ///\endcond
};

#endif //INTERPOLATION_H
