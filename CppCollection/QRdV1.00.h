/*
 * Copyright (c) 2008-2011 Zhang Ming (M. Zhang), zmjerry@163.com
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 2 or any later version.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details. A copy of the GNU General Public License is available at:
 * http://www.fsf.org/licensing/licenses
 */


/*****************************************************************************
 *                                    QRd.h
 *
 * Class template of QR decomposition for real matrix.
 *
 * For an m-by-n matrix A, the QR decomposition is an m-by-m orthogonal
 * matrix Q and an m-by-n upper triangular matrix R so that A = Q*R.
 *
 * For economy size, denotes p = min(m,n), then Q is m-by-p, and R is n-by-p,
 * this file provides the economic decomposition format.
 *
 * The QR decompostion always exists, even if the matrix does not have full
 * rank, so the constructor will never fail. The Q and R factors can be
 * retrived via the getQ() and getR() methods. Furthermore, a solve() method
 * is provided to find the least squares solution of Ax=b or AX=B using the
 * QR factors.
 *
 * Zhang Ming, 2010-01 (revised 2010-12), Xi'an Jiaotong University.
 *****************************************************************************/


#ifndef _QRD_H
#define _QRD_H


#include "Headers.h"
#include Assert_H
template <class T>
class QRd
{
public:
    //! constructor
    QRd();
    //! destructor
    ~QRd();

    //! decompose the matrix into orthogonal matrix Q and upper triangular matrix R
    void Decompose( const Mat<T> &A );

    //! Flag to denote the matrix R is of full rank, if not full rank the Ax=B of x cannot be determined.
    bool isFullRank() const;

    //! Return the orthogonal factor of the QRD factorization.
    Mat<T> Get_Q();

    //! Return the upper triangular factor of the QRD factorization.
    Mat<T> Get_R();

    //! Retreive the Householder vectors from QRD factorization
    Mat<T> Get_H();

    /**
     * @brief Least squares solution of A*X = _b
     * @param _b the determination y vector
     * @return X: a matrix that minimizes the two norm of Q*R*X-b
     * @note If b is non-conformant, or if m_qr.isFullRank() is false, the
     * routine returns a null (0) vector.
    */
    Vec<T> Solve( const Vec<T> &_b );
    
    /**
     * @brief Least squares solution of A*X = _B
     * @param _B the determination y vector
     * @return X: a matrix that minimizes the two norm of Q*R*X-B
     * @note If B is non-conformant, or if m_qr.isFullRank() is false, the
     * routine returns a null (0) matrix.
    */
    Mat<T> Solve( const Mat<T> &_B );

private:
    //! internal storage of QRD
    Mat<T> m_qr;

    //! diagonal of R.
    Vec<T> m_RDiag;

};

template<typename T>
QRd<T>::QRd()
{
}

template<typename T>
QRd<T>::~QRd()
{
}

template <typename T>
void QRd<T>::Decompose( const Mat<T> &A )
{
    int m = A.rows(), n = A.cols(), p = min(m,n);
    m_qr = A;
    m_RDiag.set_size(p);
//     m_RDiag = Vec<T>(p);

    // main loop.
    for ( int k=0; k<p; ++k )
    {
        // Compute 2-norm of k-th column without under/overflow.
        T nrm = 0;
        for ( int i=k; i<m; ++i )
            nrm = hypot( nrm, m_qr(i,k));

        if ( nrm != 0 )
        {
            // Form k-th Householder vector.
            if ( m_qr(k,k) < 0 )
                nrm = -nrm;

            for ( int i=k; i<m; ++i )
                m_qr(i,k) /= nrm;

            m_qr(k,k) += 1;

            // Apply transformation to remaining columns.
            for ( int j=k+1; j<n; ++j )
            {
                T s = 0;
                for ( int i=k; i<m; ++i )
                    s += m_qr(i,k)*m_qr(i,j);

                s = -s/m_qr(k,k);
                for ( int i=k; i<m; ++i )
                    m_qr(i,j) += s*m_qr(i,k);
            }
        }

        m_RDiag[k] = -nrm;
    }
}


template <typename T>
inline bool QRd<T>::isFullRank() const
{
    for ( int j=0; j<m_RDiag.length(); ++j )
        if ( m_RDiag[j] == 0 )
            return false;

    return true;
}

template <typename T>
Mat<T> QRd<T>::Get_Q()
{
    int m = m_qr.rows(), p = m_RDiag.length();
    Mat<T> Q( m, p );

    for ( int k=p-1; k>=0; --k )
    {
        for ( int i=0; i<m; ++i )
            Q(i,k) = 0;

        Q(k,k) = 1;
        for ( int j=k; j<p; ++j )
            if ( m_qr(k,k) != 0 )
            {
                T s = 0;
                for ( int i=k; i<m; ++i )
                    s += m_qr(i,k) * Q(i,j);

                s = -s / m_qr(k,k);
                for ( int i=k; i<m; ++i )
                    Q(i,j) += s*m_qr(i,k);
            }
    }

    return Q;
}

template <typename T>
Mat<T> QRd<T>::Get_R()
{
    int n = m_qr.cols(), p = m_RDiag.length();
    Mat<T> R( p, n );
    R.zeros();

    for ( int i=0; i<p; ++i )
        for ( int j=0; j<n; ++j )
            if ( i < j )
                R(i,j) = m_qr(i,j);
            else if ( i == j )
                R(i,j) = m_RDiag[i];
    return R;
}

template <typename T>
Mat<T> QRd<T>::Get_H()
{
    int m = m_qr.rows(),
            p = m_RDiag.length();
    Mat<T> H( m, p );

    for ( int i=0; i<m; ++i )
        for ( int j=0; j<=i&&j<p; ++j )
            H(i,j) = m_qr(i,j);

    return H;
}

template <typename T>
Vec<T> QRd<T>::Solve( const Vec<T> &b )
{
    int m = m_qr.rows(),
            n = m_qr.cols();
    Assert_Dbg( b.length() == m,"input length mismatch!");

    // matrix is rank deficient
    if ( !isFullRank() )
    {
        return Vec<T>();
    }

    Vec<T> x = b;

    // compute y = transpose(Q)*b
    for ( int k=0; k<n; ++k )
    {
        T s = 0;
        for ( int i=k; i<m; ++i )
            s += m_qr(i,k)*x[i];

        s = -s/m_qr(k,k);
        for ( int i=k; i<m; ++i )
            x[i] += s*m_qr(i,k);
    }

    // solve R*x = y;
    for ( int k=n-1; k>=0; --k )
    {
        x[k] /= m_RDiag[k];
        for ( int i=0; i<k; ++i )
            x[i] -= x[k]*m_qr(i,k);
    }

    // return n portion of x
    Vec<T> x_(n);
    for ( int i=0; i<n; ++i )
        x_[i] = x[i];

    return x_;
}

template <typename T>
Mat<T> QRd<T>::Solve( const Mat<T> &B )
{
    int m = m_qr.rows();
    int n = m_qr.cols();

    Assert( B.rows() == m, "input length mismatch!" );

    // matrix is rank deficient
    if ( !isFullRank() )
        return Mat<T>(0,0);

    int nx = B.cols();
    Mat<T> X = B;

    // compute Y = transpose(Q)*B
    for ( int k=0; k<n; ++k )
        for ( int j=0; j<nx; ++j )
        {
            T s = 0;
            for ( int i=k; i<m; ++i )
                s += m_qr(i,k)*X(i,j);

            s = -s/m_qr(k,k);
            for ( int i=k; i<m; ++i )
                X(i,j) += s*m_qr(i,k);
        }

    // solve R*X = Y;
    for ( int k=n-1; k>=0; --k )
    {
        for ( int j=0; j<nx; ++j )
            X(k,j) /= m_RDiag[k];

        for ( int i=0; i<k; ++i )
            for ( int j=0; j<nx; ++j )
                X(i,j) -= X(k,j)*m_qr(i,k);
    }

    // return n x nx portion of X
    Mat<T> X_( n, nx );
    for ( int i=0; i<n; ++i )
        for ( int j=0; j<nx; ++j )
            X_(i,j) = X(i,j);

    return X_;
}

#endif
// QRD_H
