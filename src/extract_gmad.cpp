
#include "gmadd.h"
#include "AddonWhiteList.h"
#include "AddonFormat.h"
#include "AddonReader.h"

using namespace Bootil;

int ExtractAddonFile( BString strFile, BString strOutPath, bool pauseOnError )
{
	bool quiet = CommandLine::HasSwitch( "-quiet" );

	Output::Msg( "Opening \"%s\"\n", strFile.c_str() );

	//
	// If an out path hasn't been provided, make our own
	//
	if ( strOutPath.empty() )
	{
		strOutPath = String::File::GetStripExtension( strFile );
	}

	//
	// Remove slash, add slash (enforces a slash)
	//
	String::File::FixSlashes( strOutPath );
	String::Util::TrimRight( strOutPath, "/" );
	strOutPath = strOutPath + "/";

	Addon::Reader addon;
	if ( !addon.ReadFromFile( strFile ) )
	{
		Output::Warning( "There was a problem opening the file\n" );
		if ( pauseOnError ) Pause();
		return 1;
	}

	if ( !addon.Parse() )
	{
		Output::Warning( "There was a problem parsing the file\n" );
		if ( pauseOnError ) Pause();
		return 1;
	}

	Output::Msg( "Extracting Files:\n" );
	uint32_t badFileCount = 0;
	BOOTIL_FOREACH_CONST( entry, addon.GetList(), Addon::FileEntry::List )
	{
		if ( !quiet ) Output::Msg( "\t%s [%s]\n", entry->strName.c_str(), String::Format::Memory( entry->iSize ).c_str() );
		
		// Make sure folders exists
		File::CreateFolder( strOutPath + String::File::GetStripFilename( entry->strName ), true );
		
		// Load the file into the buffer
		AutoBuffer fileContents;
		if ( addon.ReadFile( entry->iFileNumber, fileContents ) )
		{
			// Write the file to disk
			// TODO: Check if the file passes the whitelist before writing to disk?
			bool fileWriteSuccess = false;
			bool badSymbols = Bootil::String::Test::Contains( entry->strName, "./" ) || Bootil::String::Test::Contains( entry->strName, ".\\" );
			if ( !badSymbols )
			{
				BString targetFullPath = strOutPath + entry->strName;
				if ( !File::Exists( targetFullPath ) )
				{
					fileWriteSuccess = File::Write( targetFullPath, fileContents );
				}
				else
				{
					Output::Warning( "\t\tCouldn't write to '%s', file already exists?\n", targetFullPath.c_str() );
				}
			}

			// We failed for whatever reason, try again to a different folder..
			if ( !fileWriteSuccess )
			{
				BString genPath = "badnames/" + String::ToString( badFileCount ) + ".unk";
				
				if ( quiet ) { Output::Warning( "\t\tCouldn't write, trying to write as '%s'..\n", genPath.c_str() ); }
				else { Output::Warning( "\t\tCouldn't write to '%s', trying to write as '%s'..\n", entry->strName.c_str(), genPath.c_str() ); }

				// Try to write the file but don't use any of its name, since we don't know which part of it may have caused the problem
				File::CreateFolder( strOutPath + "badnames/", true );

				BString targetFullPath = strOutPath + genPath;
				if ( !File::Exists( targetFullPath ) )
				{
					if ( !File::Write( targetFullPath, fileContents ) )
					{
						Output::Warning( "\t\tCouldn't write to '%s'..\n", targetFullPath.c_str() );
					}
				}
				else
				{
					Output::Warning( "\t\tCouldn't write to '%s', file already exists?\n", targetFullPath.c_str() );
				}

				badFileCount++;
			}
		}
		else
		{
			if ( !quiet ) Output::Warning( "\t\tCouldn't extract!\n" );
			else Output::Warning( "\t\tCouldn't extract '%s'!\n", entry->strName.c_str() );
		}
	}
	Output::Msg( "Done!\n" );

	if ( pauseOnError && badFileCount > 0 ) Pause();

	return 0;
}
