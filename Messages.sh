#!/bin/sh
#
#
# invoke the extractrc script on all .ui, .rc, and .kcfg files in the sources
#
# the results are stored in a pseudo .cpp file to be picked up by xgettext.
#
$EXTRACTRC `find gui -name '*.ui' -o -name '*.rc'` `find shared -name '*.ui' -o -name '*.rc'` >> rc.cpp
$XGETTEXT rc.cpp -o $podir/kdewin-installer-gui.pot
rm -f rc.cpp
#
# if your application contains tips-of-the-day, call preparetips as well.
#
#$PREPARETIPS > tips.cpp

#
#
# convert qt messages
$XGETTEXT_QT `find gui -name '*.cpp' -o -name '*.h'` `find shared -name '*.cpp' -o -name '*.h'`  -o $podir/kdewin-installer-gui_qt.pot
