X-RDate: Wed, 16 Sep 1998 18:19:33 +0100 (IST)
Received: from mailgate.ul.ie ([136.201.1.23]) by exch-staff1.ul.ie with SMTP
 (Microsoft Exchange Internet Mail Service Version 5.5.1960.3) id Q4MF0H7K;
 Wed, 16 Sep 1998 18:04:19 +0100
Received: from sovereign.org by mailgate.ul.ie with SMTP (PP) id
 <02778-0@mailgate.ul.ie>; Wed, 16 Sep 1998 18:18:58 +0000
Received: (from jfree@localhost) by sovereign.sovereign.org
 (8.8.8/8.8.8/Debian/GNU) id LAA23077; Wed, 16 Sep 1998 11:18:09 -0600
Message-Id: <199809161718.LAA23077@sovereign.sovereign.org>
Date: Wed, 16 Sep 1998 11:18:09 -0600 (MDT)
X-Mailer: ELM [version 2.4 PL25 PGP2]
MIME-Version: 1.0
Content-Type: text/plain; charset=US-ASCII
Content-Transfer-Encoding: 7bit
XFMstatus: 0000
From: Jim Freeman <jfree@sovereign.org>
To: Caolan.McNamara@ul.ie
Subject: mswordview.spec for RPM/SRPM

Good work on mswordview!

Dunno if you've had requests for RPMs of it, but if it's of any
use, here is a .spec for creating an .i386.rpm and .src.rpm :
===============================
%define Name	mswordview
%define Version	0.4.4
%define Release	1
%define Prefix	/usr

Summary: MSWord 8 binary file format -> HTML converter
Name: %{Name}
Version: %{Version}
Release: %{Release}
Group: Utilities/Text
URL: http://www.gnu.org/~caolan/docs/MSWordView.html

Copyright: GPL
Packager: Jim Freeman <jfree@sovereign.org>

BuildRoot: /tmp/%{Name}

Source0: http://www.gnu.org/~caolan/publink/mswordview/mswordview-%{Version}.tar.gz


%Description
MSWord 8 binary file format -> HTML converter

%Prep
%setup -n %{Name}

%Build
./configure --prefix=/usr
make

%Install
DESTDIR=${RPM_BUILD_ROOT}; export DESTDIR
[ -n "`echo ${DESTDIR} | sed -n 's:^/tmp/[^.].*$:OK:p'`" ] &&
	rm -rf ${DESTDIR} ||
	(echo "Invalid BuildRoot: '${DESTDIR}'! Check this .spec ..."; exit 1) ||
	exit 1

make \
  prefix=$DESTDIR/usr \
  install


%Clean
DESTDIR=${RPM_BUILD_ROOT};export DESTDIR;[ -n "${UID}" ]&&[ "${UID}" -gt 0 ]&&exit 0
[ -n "`echo ${DESTDIR} | sed -n 's:^/tmp/[^.].*$:OK:p'`" ] && rm -rf ${DESTDIR} ||
(echo "Invalid BuildRoot: '${DESTDIR}'! Check this .spec ..."; exit 1) || exit 1
#rm -rf ${RPM_BUILD_ROOT}

%Files
/usr/bin/*
/usr/man/*/*
/usr/lib/mswordview/

%ChangeLog
