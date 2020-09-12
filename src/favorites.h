﻿/*
 *
 *  Copyright ( c ) 2011-2015
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  ( at your option ) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FAVORITES_H
#define FAVORITES_H

#include <QString>

#include <vector>
#include <forward_list>

#include "utility.h"
#include "json_parser.hpp"

class favorites
{
public:
	class triState{
	public:
		triState() : m_state( triState::STATES::UNDEFINED )
		{
		}
		triState( bool e ) : m_state( e ? STATES::TRUE : STATES::FALSE )
		{
		}
		void operator=( bool e )
		{
			if( e ){

				m_state = STATES::TRUE ;
			}else{
				m_state = STATES::FALSE ;
			}
		}
		void toggle()
		{
			if( m_state == STATES::TRUE ){

				m_state = STATES::FALSE ;

			}else if( m_state == STATES::FALSE ){

				m_state = STATES::TRUE ;
			}
		}
		bool undefined() const
		{
			return m_state == STATES::UNDEFINED ;
		}
		bool defined() const
		{
			return !this->undefined() ;
		}
		bool True() const
		{
			return m_state == STATES::TRUE ;
		}
		bool False() const
		{
			return m_state != STATES::TRUE ;
		}
		static void writeTriState( SirikaliJson& json,
					   const favorites::triState& state,
					   const char * entry )
		{
			if( state.defined() ){

				if( state.True() ){

					json[ entry ] = "true" ;
				}else{
					json[ entry ] = "false" ;
				}
			}else{
				json[ entry ] = "" ;
			}
		}
		static void readTriState( SirikaliJson& json,
					  favorites::triState& state,
					  const char * entry )
		{
			auto s = json.getString( entry ) ;

			if( !s.isEmpty() ){

				state = s == "true" ? true : false ;
			}
		}
	private:
		enum class STATES{ UNDEFINED,TRUE,FALSE } m_state ;
	};

	struct entry
	{
		entry() ;
		entry( const QString& volumePath,const QString& mountPath = QString() ) ;

		bool hasValue() const
		{
			return !volumePath.isEmpty() ;
		}

		QString volumePath ;
		QString mountPointPath ;
		QString configFilePath ;
		QString keyFile ;
		QString idleTimeOut ;
		QString identityFile ;
		QString identityAgent ;
		QString mountOptions ;
		QString preMountCommand ;
		QString postMountCommand ;
		QString preUnmountCommand ;
		QString postUnmountCommand ;
		QString internalConfigPath ;

		QByteArray password ;

		bool reverseMode = false ;
		bool volumeNeedNoPassword = false ;

		favorites::triState readOnlyMode ;
		favorites::triState autoMount ;
	};

	static favorites& instance()
	{
		static favorites m ;
		return m ;
	}

	struct volEntry{

		volEntry( const favorites::entry& e ) :
			favorite( e ),password( e.password )
		{
		}
		volEntry( const favorites::entry& e,QByteArray s ) :
			favorite( e ),password( std::move( s ) )
		{
		}
		const favorites::entry& favorite ;
		QByteArray password ;
	};

	class favoriteContainer{
	public:
		favoriteContainer() : m_iter( m_favoriteContainer.before_begin() )
		{
		}
		const favorites::entry& add( favorites::entry e )
		{
			m_iter = m_favoriteContainer.emplace_after( m_iter,std::move( e ) ) ;
			return *m_iter ;
		}
	private:
		std::forward_list< favorites::entry > m_favoriteContainer ;
		std::forward_list< favorites::entry >::iterator m_iter ;
	} ;

	using volumeList = std::vector< volEntry > ;

	enum class error{ ENTRY_ALREADY_EXISTS,FAILED_TO_CREATE_ENTRY,SUCCESS } ;

	error add( const favorites::entry& ) ;

	void replaceFavorite( const favorites::entry&, const favorites::entry& ) ;
	void removeFavoriteEntry( const favorites::entry& ) ;

	const std::vector< favorites::entry >& readFavorites() const ;

	volumeList readVolumeList() const ;

	const favorites::entry& readFavoriteByPath( const QString& configPath ) const ;

	favorites::entry readFavoriteByFileSystemPath( const QString& configPath ) const ;

	const favorites::entry& unknown() const ;

	const favorites::entry& readFavorite( const QString& volumePath,
					      const QString& mountPath = QString() ) const ;

	favorites() ;
private:
	void reload() ;
	std::vector< favorites::entry > m_favorites ;
	favorites::entry m_empty ;
};

#endif // FAVORITES_H
