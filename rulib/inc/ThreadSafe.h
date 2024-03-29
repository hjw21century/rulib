/*******************************************************************
// Copyright (c) 2000, Robert Umbehant
// mailto:rumbehant@wheresjames.com
//
// This library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU Lesser General Public 
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later 
// version.
//
// This library is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public 
// License along with this library; if not, write to the Free 
// Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
// MA 02111-1307 USA 
//
*******************************************************************/
// ThreadSafe.h: interface for the CTreadSafe class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREADSAFE_H__AFC4EF66_886A_11D2_88BC_00104B2C9CFA__INCLUDED_)
#define AFX_TREADSAFE_H__AFC4EF66_886A_11D2_88BC_00104B2C9CFA__INCLUDED_


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//==================================================================
// TThreadSafe
//
/// This template class is for thread-safeing common data types
/**
	Provides thread-safeing for common data types
*/
//==================================================================
template < class T > class TThreadSafe  
{
public:

	//==============================================================
	// TThreadSafe()
	//==============================================================
	/// Constructor
	/**
		\param [in] to	-	Lock timeout value 
	*/
	TThreadSafe( DWORD to = INFINITE ) : m_hMutex( CreateMutex( NULL, FALSE, NULL ) ) 
	{ m_dwTimeout = to; m_dwLocks = 0; }

	/// Destructor
	virtual ~TThreadSafe() { CloseHandle( m_hMutex ); }

	//==============================================================
	// settimeout()
	//==============================================================
	/// Sets the timeout value
	/**
		\param [in] to	-	Timeout value
	*/
	void settimeout( DWORD to )
	{	
		if ( lock() )
		{
			m_dwTimeout = to;
			unlock();
		} // end if
	}



	//==============================================================
	// acquire()
	//==============================================================
	/// Acquires access to the object
	/**			
		\param [in] timeout	-	Maximum time in milli-seconds to wait
								for lock.

		Calling these directly does not keep track of how many times 
		the mutex has been locked.  Therefore release() must be called
		once for every call to aquire().  Failing to do so will deadlock
		other waiting threads

		\return Non-zero if lock is acquired
	
		\see 
	*/
	BOOL acquire( DWORD timeout = MAXDWORD )
	{ 
		if ( timeout == MAXDWORD ) timeout = m_dwTimeout;

		// Aquire access to variable
		if ( WaitForSingleObject( m_hMutex, timeout ) == WAIT_TIMEOUT )
		{	return FALSE;
		} // end if
		return TRUE;
	}

	//==============================================================
	// release()
	//==============================================================
	/// Releases lock
	void release() { ReleaseMutex( m_hMutex ); }

	//==============================================================
	// acquire()
	//==============================================================
	/// Locks the mutex
	/**			
		\param [in] timeout	-	Maximum time in milli-seconds to wait
								for lock.

		\return Non-zero if lock is acquired
	
		\see 
	*/
	BOOL lock( DWORD timeout = MAXDWORD ) 
	{	if ( timeout == MAXDWORD ) timeout = m_dwTimeout;
		if ( !acquire( timeout ) ) return FALSE; m_dwLocks++; return TRUE; }

	//==============================================================
	// unlock()
	//==============================================================
	/// Calls ReleaseMutex() once for each call to lock()
	void unlock() {	if ( m_dwLocks ) { m_dwLocks--; release(); } }

	//==============================================================
	// islocked()
	//==============================================================
	/// Returns non zero if lock is acquired
	BOOL islocked() { return aquire( 0 ); }

	// Read value
	operator T()
	{	T ret;
		if ( lock() )
		{
			ret = val;
			unlock();
		} // end if
		return ret;
	}

	// Assignment
	T operator = ( T x )
	{
		if ( lock() )
		{
			val = x;
			unlock();
		} // end if
		return x;
	}

	// Compare
	bool operator == ( T x )
	{	bool ret = FALSE;
		if ( lock() )
		{
			ret = ( val == x );
			unlock();
		} // end if
		return ret;
	}
	bool operator != ( T x )
	{ return ( !( (*this) == x ) ); }

	// Used to fetch elements of an array
	// *** if T is a pointer
	T operator []( unsigned long x ) 
	{	T ret;
		if ( lock() )
		{
			ret = val[ x ];
			unlock();
		} // end if
		return ret;
	}

	// Can't use this objects address !
	T* operator *()
	{	T* ret = NULL;
		if ( lock() )
		{	ret = &val;
			unlock();
		} // end if
		return ret;
	}
	T& operator &() 
	{	T& ret;
		if ( lock() )
		{
			ret = val;
			unlock();
		} // end if
		return ret;
	}

	T& TT() 
	{	T& ret;
		if ( lock() )
		{	ret = val;
			unlock();
		} // end if
		return ret;
	}


private:

	/// Handle for the thread lock
	HANDLE	m_hMutex;

	/// The protected variable
	T	val;

	/// Lock timeout
	DWORD	m_dwTimeout;

	/// Number of acquired locks
	DWORD	m_dwLocks;
};

//==================================================================
// CTlLock
//
/// Thread locking mechanism
/**
	Encapsulates a Windows Mutex object to provide thread locking.	
*/
//==================================================================
class CTlLock
{
public:

	//==============================================================
	// CTlLock()
	//==============================================================
	/// Creates default lock object
	CTlLock() { m_bLocked = FALSE; m_dwRef = 0; m_hMutex = CreateMutex( NULL, FALSE, NULL ); }

	//==============================================================
	// CTlLock()
	//==============================================================
	/// Creates named lock object
	/**
		\param [in] pName	-	Name for mutex
	*/
	CTlLock( LPCTSTR pName ) { m_bLocked = FALSE; m_dwRef = 0; m_hMutex = CreateMutex( NULL, FALSE, pName ); }

	/// Destructor
	~CTlLock() { if ( m_hMutex != NULL ) { CloseHandle( m_hMutex ); m_hMutex = NULL; } }

	//==============================================================
	// Lock()
	//==============================================================
	/// Locks the object
	/**
		\param [in] timeout		-	Maximum time in milli-seconds to
									wait for lock

		\return Non-zero if success
	*/
	BOOL Lock( DWORD timeout = 3000 ) 
	{	m_bLocked = ( WaitForSingleObject( m_hMutex, timeout ) != WAIT_TIMEOUT );
		if ( m_bLocked ) m_dwRef++; 
		else 
			TRACE( "Lock Failed!\n" );
		return m_bLocked; 
	}

	//==============================================================
	// Unlock()
	//==============================================================
	/// Unlocks the object
	BOOL Unlock() 
	{	ReleaseMutex( m_hMutex );
		if ( m_dwRef ) 
		{	if ( !( --m_dwRef ) ) m_bLocked = FALSE; }
		return !m_bLocked;
	}

	//==============================================================
	// IsLocked()
	//==============================================================
	/// Returns non-zero if locked
	/**
		Checks to see if the object is locked.  Does not mean the 
		caller owns the lock
	*/
	BOOL IsLocked() { return m_bLocked; }
	
private:

	/// Number of times locked
	DWORD				m_dwRef;

	/// Handle to mutex object
	HANDLE				m_hMutex;

	/// Non-zero if locked
	BOOL				m_bLocked;
};

//==================================================================
// CTlLocalLock
//
/// Use this to lock and automatically unlock CTlLock objects
/**
	Use this to lock and automatically unlock CTlLock objects
*/
//==================================================================
class CTlLocalLock
{

public:

	/// Default constructor
	CTlLocalLock() { m_ptr = NULL; }

	//==============================================================
	// CTlLocalLock()
	//==============================================================
	/// Constructor - Takes a CTlLock pointer
	/**
		\param [in] ptr			-	Pointer to CTlLock object
		\param [in] timeout		-	Maximum time in milli-seconds to
									wait for lock.
	*/
	CTlLocalLock( CTlLock *ptr, DWORD timeout = 3000 ) 
	{	m_ptr = NULL; if ( ptr != NULL ) if ( ptr->Lock( timeout ) ) m_ptr = ptr; }

	//==============================================================
	// CTlLocalLock()
	//==============================================================
	/// Constructor - Takes a CTlLock reference
	/**
		\param [in] lock		-	Reference to CTlLock object
		\param [in] timeout		-	Maximum time in milli-seconds to
									wait for lock.
	*/
	CTlLocalLock( CTlLock &lock, DWORD timeout = 3000 ) 
	{	m_ptr = NULL; if ( lock.Lock( timeout ) ) m_ptr = &lock; }

	/// Destructor - Unlocks the underlying lock
	~CTlLocalLock() { Unlock(); }

	//==============================================================
	// IsLocked()
	//==============================================================
	/// Returns true if the local object is locked
	BOOL IsLocked() { return ( m_ptr != NULL ); }

	//==============================================================
	// Attach()
	//==============================================================
	/// Attaches to an existing CTlLock without locking
	void Attach( CTlLock *ptr ) { Unlock(); m_ptr = ptr; }

	//==============================================================
	// Detach()
	//==============================================================
	/// Detaches from CTlLock without unlocking
	void Detach() { m_ptr = NULL; }

	//==============================================================
	// Lock()
	//==============================================================
	/// Locks a CTlLock object.  Returns true only if lock was achieved
	/**
		\param [in] ptr			-	Pointer to CTlLock object
		\param [in] timeout		-	Maximum time in milli-seconds to
									wait for lock.
		
		\return Non-zero if lock was acquired.
	
		\see 
	*/
	BOOL Lock( CTlLock *ptr, DWORD timeout = 3000 )
	{	if ( ptr == m_ptr ) return TRUE;
		if ( ptr != NULL ) { Unlock(); m_ptr = NULL; }
		if ( ptr == NULL ) return FALSE;
		if ( ptr->Lock( timeout ) ) m_ptr = ptr;
		return IsLocked();
	}

	//==============================================================
	// Unlock()
	//==============================================================
	/// Unlocks attached CTlLock object
	/**
		\return Always returns non-zero
	*/
	BOOL Unlock()
	{	if ( m_ptr == NULL ) return TRUE;
		m_ptr->Unlock(); m_ptr = NULL;
		return TRUE;
	}

private:

	/// Pointer to CTlLock object
	CTlLock		*m_ptr;

};


#endif // !defined(AFX_TREADSAFE_H__AFC4EF66_886A_11D2_88BC_00104B2C9CFA__INCLUDED_)
