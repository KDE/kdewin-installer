/****************************************************************************
**
** Copyright (C) 2006-2008 Ralf Habacker <ralf.habacker@freenet.de>. 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License version 2 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public License
** along with this library; see the file COPYING.LIB.  If not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA 02110-1301, USA.
**
****************************************************************************/

#include "mirrors.h"
#include "downloader.h"
#include "downloaderprogress.h"
#include "settings.h"

#include <QtDebug>
#include <QBuffer>
#include <QFile>
#include <QUrl>
#include <QStringList>
#include <QFileInfo>

QHash<QString,QString> Mirrors::m_countries;
QHash<QString,QString> Mirrors::m_continents;


Mirrors::Mirrors()
{
    initCountries();
}

Mirrors::Mirrors(const Config &config)
: m_config(config)
{
    initCountries();
}

Mirrors::~Mirrors()
{
    clear();
}

bool Mirrors::fetch()
{
#ifdef DEBUG
    QString out = "mirrors.html";
#else
    QByteArray out;
#endif
    QFileInfo cfr(Settings::instance().downloadDir()+"/mirrors-remote.lst");
    if (Settings::hasDebug("Mirrors"))
        qDebug() << "Check if a copy of the remote config file is available at" << cfr.absoluteFilePath() << (cfr.exists() ? "... found" : "... not found");
    if (cfr.exists())
    {
#ifdef DEBUG
        out = cfr.absolutePath();
#else
        QFile f(cfr.absoluteFilePath());
        if (f.open(QFile::ReadOnly)) 
        {
            out = f.readAll();
            f.close();
        }
        else 
        {
          qCritical() << "could not open" << cfr.absoluteFilePath() << "with error" << f.errorString();
          return false;
        }
#endif
     }   
     else if (!Downloader::instance()->fetch(m_config.url,out))
        return false;
    return parse(out);
}

bool Mirrors::parse(const QString &fileName)
{
    QFile file(fileName);
    if (!file.exists())
        return false;

    if (!file.open(QIODevice::ReadOnly))
        return false;
    return parse(&file);
}

bool Mirrors::parse(const QByteArray &data)
{
    QByteArray ba(data);
    QBuffer buf(&ba);

    if (!buf.open(QIODevice::ReadOnly| QIODevice::Text))
        return false;
    return parse(&buf);
}

bool Mirrors::parse(QIODevice *ioDev)
{
    m_mirrors.clear();
    switch (m_config.type) {
    case KDE:
        {
            while (!ioDev->atEnd())
            {
                const QString line = QString::fromUtf8(ioDev->readLine().replace("\n",""));
                if ( line.startsWith(QLatin1Char('#')) || line.contains("stable only") ) 
                    continue;
                const QStringList parts = line.split(QLatin1Char(' '),QString::SkipEmptyParts);
                if (parts.size() >= 3) 
                {
                    MirrorType mirror;
                    if (parts[2].contains(m_config.excludePattern))
                        mirror.url = QUrl(parts[2]);
                    else
                        mirror.url = QUrl(parts[2] + m_config.releasePath);
                    mirror.name = parts[0] + "://" + mirror.url.host();
                    if (m_countries.contains(parts[1])) 
                    {
                        mirror.country = m_countries[parts[1]];
                        if ( m_continents.contains(parts[1]) )
                            mirror.continent = m_continents[parts[1]];
                        else
                        {
                            mirror.continent = "";
                            qWarning() << "country code " << parts[1] << "not found in continent list";
                        }
                    }
                    else  
                    {
                        mirror.country = parts[1];
                        qWarning() << "country code " << parts[1] << "not found in country list";
                    }
                    m_mirrors.append(mirror);
                }
            }
            qDebug() << m_mirrors;
        }
        break;

    case Cygwin:
        {
            while (!ioDev->atEnd())
            {
                const QByteArray line = ioDev->readLine().replace("\n","").trimmed();
                if (line.startsWith("#") || line.size() == 0)
                    continue;
                const QList<QByteArray> a = line.split(';');
                if (a.size() < 4) 
                {
                    qWarning() << "invalid mirror list entry" << line << "-> ignored";
                    continue; 
                }
                MirrorType mirror;
                if (a[0].contains(m_config.excludePattern.toLatin1()))
                    mirror.url = a[0];
                else
                    mirror.url = QUrl(a[2] + m_config.releasePath);

                mirror.name = a[1];
                mirror.continent = a[2];
                mirror.country = a[3];
                m_mirrors.append(mirror);
            }
        }
        break;
    }
    return true;
}

Mirrors &Mirrors::instance()
{
    static Mirrors mirrors;
    return mirrors;
}

QDebug &operator<<(QDebug &out, const MirrorTypeList &c)
{
    out << "QList<MirrorType> (";
    Q_FOREACH(const MirrorType &m,c)
        out << m;
    out << ")";
    return out;
}

QDebug &operator<<(QDebug &out, const MirrorType &c)
{
    out << "MirrorType ("
        << "url:" << c.url
        << "name:" << c.name
        << "continent:" << c.continent
        << "country:" << c.country
        << ")";
    return out;
}

void Mirrors::initCountries()
{
    if (m_countries.size() > 0)
        return;
    m_countries["ad"] = "Andorra";
    m_countries["ae"] = "United Arab Emirates";
    m_countries["af"] = "Afghanistan";
    m_countries["ag"] = "Antigua And Barbuda";
    m_countries["ai"] = "Anguilla";
    m_countries["al"] = "Albania";
    m_countries["am"] = "Armenia";
    m_countries["an"] = "Netherlands Antilles";
    m_countries["ao"] = "Angola";
    m_countries["aq"] = "Antarctica";
    m_countries["ar"] = "Argentina";
    m_countries["as"] = "American Samoa";
    m_countries["at"] = "Austria";
    m_countries["au"] = "Australia";
    m_countries["aw"] = "Aruba";
    m_countries["az"] = "Azerbaijan";
    m_countries["ba"] = "Bosnia And Herzegovina";
    m_countries["bb"] = "Barbados";
    m_countries["bd"] = "Bangladesh";
    m_countries["be"] = "Belgium";
    m_countries["bf"] = "Burkina Faso";
    m_countries["bg"] = "Bulgaria";
    m_countries["bh"] = "Bahrain";
    m_countries["bi"] = "Burundi";
    m_countries["bj"] = "Benin";
    m_countries["bm"] = "Bermuda";
    m_countries["bn"] = "Brunei Darussalam";
    m_countries["bo"] = "Bolivia";
    m_countries["br"] = "Brazil";
    m_countries["bs"] = "Bahamas";
    m_countries["bt"] = "Bhutan";
    m_countries["bv"] = "Bouvet Island";
    m_countries["bw"] = "Botswana";
    m_countries["by"] = "Belarus";
    m_countries["bz"] = "Belize";
    m_countries["ca"] = "Canada";
    m_countries["cc"] = "Cocos (Keeling) Islands";
    m_countries["cd"] = "Congo, The Democratic Republic Of The";
    m_countries["cf"] = "Central African Republic";
    m_countries["cg"] = "Congo";
    m_countries["ch"] = "Switzerland";
    m_countries["ci"] = "Cote D'Ivoire";
    m_countries["ck"] = "Cook Islands";
    m_countries["cl"] = "Chile";
    m_countries["cm"] = "Cameroon";
    m_countries["cn"] = "China";
    m_countries["co"] = "Colombia";
    m_countries["cr"] = "Costa Rica";
    m_countries["cu"] = "Cuba";
    m_countries["cv"] = "Cape Verde";
    m_countries["cx"] = "Christmas Island";
    m_countries["cy"] = "Cyprus";
    m_countries["cz"] = "Czech Republic";
    m_countries["de"] = "Germany";
    m_countries["dj"] = "Djibouti";
    m_countries["dk"] = "Denmark";
    m_countries["dm"] = "Dominica";
    m_countries["do"] = "Dominican Republic";
    m_countries["dz"] = "Algeria";
    m_countries["ec"] = "Ecuador";
    m_countries["ee"] = "Estonia";
    m_countries["eg"] = "Egypt";
    m_countries["eh"] = "Western Sahara";
    m_countries["er"] = "Eritrea";
    m_countries["es"] = "Spain";
    m_countries["et"] = "Ethiopia";
    m_countries["fi"] = "Finland";
    m_countries["fj"] = "Fiji";
    m_countries["fk"] = "Falkland Islands (Malvinas)";
    m_countries["fm"] = "Federated States of Micronesia";
    m_countries["fo"] = "Faroe Islands";
    m_countries["fr"] = "France";
    m_countries["ga"] = "Gabon";
    m_countries["gb"] = "United Kingdom";
    m_countries["gd"] = "Grenada";
    m_countries["ge"] = "Georgia";
    m_countries["gf"] = "French Guiana";
    m_countries["gh"] = "Ghana";
    m_countries["gi"] = "Gibraltar";
    m_countries["gl"] = "Greenland";
    m_countries["gm"] = "Gambia";
    m_countries["gn"] = "Guinea";
    m_countries["gp"] = "Guadeloupe";
    m_countries["gq"] = "Equatorial Guinea";
    m_countries["gr"] = "Greece";
    m_countries["gs"] = "South Georgia And The South Sandwich Islands";
    m_countries["gt"] = "Guatemala";
    m_countries["gu"] = "Guam";
    m_countries["gw"] = "Guinea-Bissau";
    m_countries["gy"] = "Guyana";
    m_countries["hk"] = "Hong Kong";
    m_countries["hm"] = "Heard Island And Mcdonald Islands";
    m_countries["hn"] = "Honduras";
    m_countries["hr"] = "Croatia";
    m_countries["ht"] = "Haiti";
    m_countries["hu"] = "Hungary";
    m_countries["id"] = "Indonesia";
    m_countries["ie"] = "Ireland";
    m_countries["il"] = "Israel";
    m_countries["in"] = "India";
    m_countries["io"] = "British Indian Ocean Territory";
    m_countries["iq"] = "Iraq";
    m_countries["ir"] = "Iran, Islamic Republic of";
    m_countries["is"] = "Iceland";
    m_countries["it"] = "Italy";
    m_countries["jm"] = "Jamaica";
    m_countries["jo"] = "Jordan";
    m_countries["jp"] = "Japan";
    m_countries["ke"] = "Kenya";
    m_countries["kg"] = "Kyrgyzstan";
    m_countries["kh"] = "Cambodia";
    m_countries["ki"] = "Kiribati";
    m_countries["km"] = "Comoros";
    m_countries["kn"] = "Saint Kitts And Nevis";
    m_countries["kp"] = "Democratic People's Republic of Korea";
    m_countries["kr"] = "Republic of Korea";
    m_countries["kw"] = "Kuwait";
    m_countries["ky"] = "Cayman Islands";
    m_countries["kz"] = "Kazakhstan";
    m_countries["la"] = "Lao People's Democratic Republic";
    m_countries["lb"] = "Lebanon";
    m_countries["lc"] = "Saint Lucia";
    m_countries["li"] = "Liechtenstein";
    m_countries["lk"] = "Sri Lanka";
    m_countries["lr"] = "Liberia";
    m_countries["ls"] = "Lesotho";
    m_countries["lt"] = "Lithuania";
    m_countries["lu"] = "Luxembourg";
    m_countries["lv"] = "Latvia";
    m_countries["ly"] = "Libyan Arab Jamahiriya";
    m_countries["ma"] = "Morocco";
    m_countries["mc"] = "Monaco";
    m_countries["md"] = "Moldova, Republic of";
    m_countries["mg"] = "Madagascar";
    m_countries["mh"] = "Marshall Islands";
    m_countries["mk"] = "Macedonia";
    m_countries["ml"] = "Mali";
    m_countries["mm"] = "Myanmar";
    m_countries["mn"] = "Mongolia";
    m_countries["mo"] = "Macao";
    m_countries["mp"] = "Northern Mariana Islands";
    m_countries["mq"] = "Martinique";
    m_countries["mr"] = "Mauritania";
    m_countries["ms"] = "Montserrat";
    m_countries["mt"] = "Malta";
    m_countries["mu"] = "Mauritius";
    m_countries["mv"] = "Maldives";
    m_countries["mw"] = "Malawi";
    m_countries["mx"] = "Mexico";
    m_countries["my"] = "Malaysia";
    m_countries["mz"] = "Mozambique";
    m_countries["na"] = "Namibia";
    m_countries["nc"] = "New Caledonia";
    m_countries["ne"] = "Niger";
    m_countries["nf"] = "Norfolk Island";
    m_countries["ng"] = "Nigeria";
    m_countries["ni"] = "Nicaragua";
    m_countries["nl"] = "Netherlands";
    m_countries["no"] = "Norway";
    m_countries["np"] = "Nepal";
    m_countries["nr"] = "Nauru";
    m_countries["nu"] = "Niue";
    m_countries["nz"] = "New Zealand";
    m_countries["om"] = "Oman";
    m_countries["pa"] = "Panama";
    m_countries["pe"] = "Peru";
    m_countries["pf"] = "French Polynesia";
    m_countries["pg"] = "Papua New Guinea";
    m_countries["ph"] = "Philippines";
    m_countries["pk"] = "Pakistan";
    m_countries["pl"] = "Poland";
    m_countries["pm"] = "Saint Pierre And Miquelon";
    m_countries["pn"] = "Pitcairn";
    m_countries["pr"] = "Puerto Rico";
    m_countries["ps"] = "Palestinian Territory, Occupied";
    m_countries["pt"] = "Portugal";
    m_countries["pw"] = "Palau";
    m_countries["py"] = "Paraguay";
    m_countries["qa"] = "Qatar";
    m_countries["re"] = "Reunion";
    m_countries["ro"] = "Romania";
    m_countries["ru"] = "Russian Federation";
    m_countries["rw"] = "Rwanda";
    m_countries["sa"] = "Saudi Arabia";
    m_countries["sb"] = "Solomon Islands";
    m_countries["sc"] = "Seychelles";
    m_countries["sd"] = "Sudan";
    m_countries["se"] = "Sweden";
    m_countries["sg"] = "Singapore";
    m_countries["sh"] = "Saint Helena";
    m_countries["si"] = "Slovenia";
    m_countries["sj"] = "Svalbard And Jan Mayen";
    m_countries["sk"] = "Slovakia";
    m_countries["sl"] = "Sierra Leone";
    m_countries["sm"] = "San Marino";
    m_countries["sn"] = "Senegal";
    m_countries["so"] = "Somalia";
    m_countries["sr"] = "Suriname";
    m_countries["st"] = "Sao Tome And Principe";
    m_countries["sv"] = "El Salvador";
    m_countries["sy"] = "Syrian Arab Republic";
    m_countries["sz"] = "Swaziland";
    m_countries["tc"] = "Turks And Caicos Islands";
    m_countries["td"] = "Chad";
    m_countries["tf"] = "French Southern Territories";
    m_countries["tg"] = "Togo";
    m_countries["th"] = "Thailand";
    m_countries["tj"] = "Tajikistan";
    m_countries["tk"] = "Tokelau";
    m_countries["tl"] = "Timor-Leste";
    m_countries["tm"] = "Turkmenistan";
    m_countries["tn"] = "Tunisia";
    m_countries["to"] = "Tonga";
    m_countries["tr"] = "Turkey";
    m_countries["tt"] = "Trinidad And Tobago";
    m_countries["tv"] = "Tuvalu";
    m_countries["tw"] = "Taiwan";
    m_countries["tz"] = "Tanzania, United Republic of";
    m_countries["ua"] = "Ukraine";
    m_countries["ug"] = "Uganda";
    m_countries["uk"] = "United Kingdom";
    m_countries["um"] = "United States Minor Outlying Islands";
    m_countries["us"] = "United States";
    m_countries["uy"] = "Uruguay";
    m_countries["uz"] = "Uzbekistan";
    m_countries["va"] = "Holy See (Vatican City State)";
    m_countries["vc"] = "Saint Vincent And The Grenadines";
    m_countries["ve"] = "Venezuela";
    m_countries["vg"] = "Virgin Islands, British";
    m_countries["vi"] = "Virgin Islands, U.S.";
    m_countries["vn"] = "Viet Nam";
    m_countries["vu"] = "Vanuatu";
    m_countries["wf"] = "Wallis And Futuna";
    m_countries["ws"] = "Samoa";
    m_countries["ye"] = "Yemen";
    m_countries["yt"] = "Mayotte";
    m_countries["yu"] = "Yugoslavia";
    m_countries["za"] = "South Africa";
    m_countries["zm"] = "Zambia";
    m_countries["zw"] = "Zimbabwe";
    m_countries["--"] = "";

    // setup countryGroups hash table 
    QHash<QString,QString> countryGroups;
    countryGroups["Africa"] = "na:za";
    countryGroups["Northern America"] = "ca:mx:us";
    countryGroups["Central America"] = "cr";
    countryGroups["Southern America"] = "ar:cl"; 
    countryGroups["Asia & Oceania"] = "au:cn:hk:id:il:jp:kr:my:sa:sg:th:tw:zh";
    countryGroups["Central Europe"] = "at:ch:cz:de:hu:lt:pl:si:sk";
    countryGroups["Eastern Europe"] = "am:az:bg:by:ge:lv:md:ro:ru:ua";
    countryGroups["Northern Europe"] = "dk:ee:is:fi:no:se";
    countryGroups["Southern Europe"] = "ba:cy:gr:hr:it:mk:mt:va:yu:tr";
    countryGroups["Western Europe"] = "be:es:fr:ie:lu:nl:pt:uk";
    countryGroups["World Wide"] = "--";

    // setup m_continents hash table 
    // which is accessable by using QString continent = m_continents[<countrycode>];
    QHash<QString, QString>::ConstIterator it = countryGroups.constBegin();
    for( ; it != countryGroups.constEnd(); ++it) {
        const QStringList countries = it.value().split(':');
        Q_FOREACH(const QString &country, countries) 
            m_continents[country] = it.key();
    }
    qDebug() << m_continents;
}
