/***************************************************************************
                          qgsserverrequest.cpp

  Define ruquest class for getting request contents
  -------------------
  begin                : 2016-12-05
  copyright            : (C) 2016 by David Marteau
  email                : david dot marteau at 3liz dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsserverrequest.h"
#include <QUrlQuery>


QgsServerRequest::QgsServerRequest( const QString &url, Method method, const Headers &headers )
  : QgsServerRequest( QUrl( url ), method, headers )
{
}

QgsServerRequest::QgsServerRequest( const QUrl &url, Method method, const Headers &headers )
  : mUrl( url )
  , mOriginalUrl( url )
  , mBaseUrl( url )
  , mMethod( method )
  , mHeaders( headers )
  , mRequestHeaderConv()
{
  mRequestHeaderConv.insert( HOST, QStringLiteral( "Host" ) );
  mRequestHeaderConv.insert( FORWARDED, QStringLiteral( "Forwarded" ) );
  mRequestHeaderConv.insert( X_FORWARDED_FOR, QStringLiteral( "X-Forwarded-For" ) );
  mRequestHeaderConv.insert( X_FORWARDED_HOST, QStringLiteral( "X-Forwarded-Host" ) );
  mRequestHeaderConv.insert( X_FORWARDED_PROTO, QStringLiteral( "X-Forwarded-Proto" ) );
  mRequestHeaderConv.insert( X_QGIS_SERVICE_URL, QStringLiteral( "X-Qgis-Service-Url" ) );
  mRequestHeaderConv.insert( X_QGIS_WMS_SERVICE_URL, QStringLiteral( "X-Qgis-Wms-Service-Url" ) );
  mRequestHeaderConv.insert( X_QGIS_WFS_SERVICE_URL, QStringLiteral( "X-Qgis-Wfs-Service-Url" ) );
  mRequestHeaderConv.insert( X_QGIS_WCS_SERVICE_URL, QStringLiteral( "X-Qgis-Wcs-Service-Url" ) );
  mRequestHeaderConv.insert( X_QGIS_WMTS_SERVICE_URL, QStringLiteral( "X-Qgis-Wmts-Service-Url" ) );

  mParams.load( QUrlQuery( url ) );
}

QgsServerRequest::QgsServerRequest( const QgsServerRequest &other )
  : mUrl( other.mUrl )
  , mOriginalUrl( other.mOriginalUrl )
  , mBaseUrl( other.mBaseUrl )
  , mMethod( other.mMethod )
  , mHeaders( other.mHeaders )
  , mParams( other.mParams )
  , mRequestHeaderConv( other.mRequestHeaderConv )
{
}

QString QgsServerRequest::methodToString( const QgsServerRequest::Method &method )
{
  static const QMetaEnum metaEnum = QMetaEnum::fromType<QgsServerRequest::Method>();
  return QString( metaEnum.valueToKey( method ) ).remove( QStringLiteral( "Method" ) ).toUpper( );
}

QString QgsServerRequest::header( const QString &name ) const
{
  return mHeaders.value( name );
}


QString QgsServerRequest::header( const QgsServerRequest::RequestHeader &headerEnum ) const
{
  return header( mRequestHeaderConv[ headerEnum ] );
}

void QgsServerRequest::setHeader( const QString &name, const QString &value )
{
  mHeaders.insert( name, value );
}

QMap<QString, QString> QgsServerRequest::headers() const
{
  return mHeaders;
}

void QgsServerRequest::removeHeader( const QString &name )
{
  mHeaders.remove( name );
}

QUrl QgsServerRequest::url() const
{
  return mUrl;
}

QUrl QgsServerRequest::originalUrl() const
{
  return mOriginalUrl;
}

void QgsServerRequest::setOriginalUrl( const QUrl &url )
{
  mOriginalUrl = url;
}

QUrl QgsServerRequest::baseUrl() const
{
  return mBaseUrl;
}

void QgsServerRequest::setBaseUrl( const QUrl &url )
{
  mBaseUrl = url;
}

QgsServerRequest::Method QgsServerRequest::method() const
{
  return mMethod;
}

QMap<QString, QString> QgsServerRequest::parameters() const
{
  return mParams.toMap();
}

QgsServerParameters QgsServerRequest::serverParameters() const
{
  return mParams;
}

QByteArray QgsServerRequest::data() const
{
  return QByteArray();
}

void QgsServerRequest::setParameter( const QString &key, const QString &value )
{
  mParams.add( key, value );
  mUrl.setQuery( mParams.urlQuery() );
}

QString QgsServerRequest::parameter( const QString &key, const QString &defaultValue ) const
{
  const auto value { mParams.value( key ) };
  if ( value.isEmpty() )
  {
    return defaultValue;
  }
  return value;
}

void QgsServerRequest::removeParameter( const QString &key )
{
  mParams.remove( key );
  mUrl.setQuery( mParams.urlQuery() );
}

void QgsServerRequest::setUrl( const QUrl &url )
{
  mUrl = url;
  mParams.clear();
  mParams.load( QUrlQuery( mUrl ) );
}

void QgsServerRequest::setMethod( Method method )
{
  mMethod = method;
}

const QString QgsServerRequest::queryParameter( const QString &name, const QString &defaultValue ) const
{
  if ( !QUrlQuery( mUrl ).hasQueryItem( name ) )
  {
    return defaultValue;
  }
  return QUrl::fromPercentEncoding( QUrlQuery( mUrl ).queryItemValue( name ).toUtf8() );
}
