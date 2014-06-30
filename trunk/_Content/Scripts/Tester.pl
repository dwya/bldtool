#!/usr/bin/perl -w
use strict;
use Win32;
use Win32::OLE;

Win32::MsgBox('Tester.pl');

##########################################################
# $ARGV[0]	is the project id, it's a string like; BuildIt.GUID
#			it is used to find the running build xml
#			there are 3 build xml seqments avaliable
#			-	BuildIt.GUID.Properties
#			-	BuildIt.GUID.Sequence
#			-	BuildIt.GUID.Projeect	(the actions are in this one)
#			-	examples to access them are below
#
# $ARGV[1]	is the name of the action being run
#
my $sProjectId	= $ARGV[0];
my $sAction		= $ARGV[1];


##########################################################
# Use ProjSrv.rBldProperties.1 to access properties
#
my $xml = Win32::OLE->new('ProjSrv.rBldProperties.1', 'UnInit');

#'BuildIt.GUID.Properties'
$xml->Init("${sProjectId}.Properties");


# not 100% sure why be we need to call SetProperty through the Invoke method,
# I suspect perl's Win32::OLE module must already have a SetProperty() method?
# possible solution is to just change the name from ProjSrv.dll
# something like xml->GetBuildProperty() xml->GetBuildProperty()
# or we 'wrap' these calls in a custom perl module, then perl can have any name it wants

my @arg = ('TestPropertyPerl', 'Value set in pear script');
$xml->Invoke('SetProperty', @arg);

Win32::MsgBox($xml->GetProperty('TestPropertyPerl'));
Win32::MsgBox($xml->GetProperty('WindowsFolder'));

$xml->UnInit();	#once UnInit() is called, 
				#$xml prperties can't be used until you call Init() again
				#you should call UnInit() before the script ends


##########################################################
# ProjSrv.rBldProjectXml.1 to access the xml actions or sequences
#
my $project = Win32::OLE->new('ProjSrv.rBldProjectXml.1', 'UnInit');

#'BuildIt.GUID.Project' has the actions
#'BuildIt.GUID.Sequence' is the running sequence
$project->Init("${sProjectId}.Project");

#select the action (data) you want to access via a XPath
my $sQuery = "//BuildIt/Actions/Action[\@name=\'${sAction}\']";
$project->Select($sQuery);

#once the node is selected, get or set attributes
#
Win32::MsgBox($project->GetAttribute('script'));

$project->SetAttribute("script01", "test setting new value - script01, in perl");
Win32::MsgBox($project->GetAttribute("script01"));
Win32::MsgBox($project->GetXPath());
Win32::MsgBox($project->GetAttribute("script"));
Win32::MsgBox($project->GetXmlFragment());

#to loop all actions, change the Select() and use CounterNext() in a loop
#example... select all actions
#

my $sQuery = "//BuildIt/Actions/Action";
$project->Select($sQuery);

do
{
	#each time through the loop we will be 
	#accessing a different node in the selection
	
	Win32::MsgBox($project->GetAttribute("name"));
	
} while ($project->CounterNext());

$project->UnInit();

Win32::MsgBox('Done');

#######################################
#
#	-	By default perl exit's with 0 (which is success)
#	-	If there is a problem and you need to fall to the
#		'failaction' (retry, ignore, or quit) -- then exit
#		with something other than 0, this becomes the return code
#		for the action.  The window's text message associated with
#		the error is written to the log file
#		http://msdn.microsoft.com/en-us/library/ms681382(VS.85).aspx
#
#	exit 2; #ERROR_FILE_NOT_FOUND

exit 0; #ERROR_SUCCESS

#################################################
#	See: ProjSrv.idl for supported methods
#
#	interface IrBldProjectXml	is prog id:	'ProjSrv.rBldProjectXml.1'
#	interface IrBldProperties	is prog id:	'ProjSrv.rBldProperties.1'
#
