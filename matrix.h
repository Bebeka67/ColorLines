#ifndef MATRIX_H
#define MATRIX_H

#include <QVector>

template<class T>
class Matrix {
    int m_rows;
    int m_columns;
    QVector<T> *m_v;
public:
    Matrix() {
        m_v = new QVector<T>;
    };
    Matrix(int r, int c, const T& val = T()) :
        m_rows(r),
        m_columns(c)
    {
        m_v = new QVector<T>(r * c, val);
    }
    ~Matrix() {
        delete[] m_v;
    }

    inline int rows() const { return m_rows; }
    inline int columns() const { return m_columns; }

    T& at(int r, int c) {
        int pos = r * m_rows + c;
        Q_ASSERT_X(pos >= 0 && pos < m_v->size(), "Matrix<T>::at", "index out of range");
        return const_cast<T&>(m_v->at(pos));
    }
    inline T& at(QPoint p) {
        return this->at(p.x(), p.y());
    }

    void fill(const T& val) {
        for(auto it = m_v->begin(); it != m_v->end(); it++)
            *it = val;
    }
};

#endif // MATRIX_H
