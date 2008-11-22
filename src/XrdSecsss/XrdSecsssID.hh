#ifndef __SecsssID__
#define __SecsssID__
/******************************************************************************/
/*                                                                            */
/*                        X r d S e c s s s I D . h h                         */
/*                                                                            */
/* (c) 2008 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
//       $Id$

#include <string.h>
#include <time.h>

#include "XrdOuc/XrdOucHash.hh"
#include "XrdSec/XrdSecEntity.hh"
#include "XrdSys/XrdSysPthread.hh"

// The XrdSecsssID class allows you to establish a registery to map loginid's
// to arbitrary entities. By default, the sss security protocol uses the
// loginid as the authenticated username and, if possible, the corresponding
// primary group membership of loginid (note that if loginid is a number,
// it is treated as a uid not as a username). By creating an instance of this
// class you can over-ride the default and map loginid's to arbitrary entities
// using the Register() method. You must create one, and only one, such
// instance prior to making any contact with a sss security enabled server.

// In order to include XrdSecsssID methods, you should either link with
// libXrdSecsss.so (preferable) or include XrdSecsssID.o and link with
// libXrdOuc.a and libXrdSys.a.

class XrdSecsssID
{
public:

// Register() creates a mapping from a loginid to an entity description. Only
//            name, vo, role, group, and endorements pointers in XrdSecEntity
//            are supported. To de-register a loginid, make the Ident arg zero.
//            To replace an existing entry, specify 1 for doReplace argument.
//            TRUE is returned if successful; FALSE otherwise (including the
//            case where idDynamic was not specified in the constructor or
//            doReplace is zero and the loginid has already been registered).
//
int      Register(const char *loginid, XrdSecEntity *Ident, int doReplace=0);

// Find() is an internal look-up method that returns the identification
//        string in the provided buffer corresponding to the loginid.
//        If loginid is registered and the data will fit into the buffer the
//        length moved into the buffer is returned. Otherwise, 0 is returned.
//
int      Find(const char *loginid, char *Buff, int Blen);

// getID() returns the static ID. If none, a null pointer is returned
//
static
const char *getID(int &idLen);

// A single instance of this class may be instantiated. The first parameter
// indicates how authentication is to be handled. The second parameter provides
// either a fixed or default authenticated identity under control of the aType
// parameter, as follows:
//
enum authType {idLogin   = 0, // 1Sided: loginid used by server as identity
                              //         Ident parameter is ignored.
                              //         Default if XrdSecsssID not instantiated!
               idMutual  = 1, // Mutual: authentication loginid is the identity
                              //         Ident parameter is ignored.
               idDynamic = 3, // Mutual: Map loginid to registered identity
                              //         Ident is default; if 0 loginid used
               idStatic  = 4, // 1Sided: fixed identity sent to the server
                              //         Ident as specified; if 0 process uid/gid
               idStaticM = 5  // Mutual: fixed identity sent to the server
                              //         Ident as specified; if 0 process uid/gid
              };

// getObj() gets the address of a previous created instance of this object or
//          zero if no instance exists in objP and returns the authType.
//
static
authType getObj(XrdSecsssID **objP);

       XrdSecsssID(authType aType=idLogin, XrdSecEntity *Ident=0);

      ~XrdSecsssID() {}

private:

struct sssID {int iLen; char iData[1];}; // Sized appropriately
static sssID *genID(const char *lid);
static sssID *genID(XrdSecEntity *eP);

static XrdSysMutex InitMutex;
static sssID      *defaultID;
XrdSysMutex        myMutex;
XrdOucHash<sssID>  Registry;
authType           myAuth;
};
#endif