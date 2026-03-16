class CfgPatches
{
	class DayZular_Script
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]={"DZ_Data"};
	};
};
class CfgMods
{
	class DayZular
	{
		dir="DayZular";
		picture="";
		action="";
		hideName=0;
		hidePicture=0;
		name="DayZular";
		credits="TheDmitri";
		author="TheDmitri";
		authorID="0";
		version="2.0";
		extra=0;
		type="mod";
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"DayZular/scripts/Common",
					"DayZular/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"DayZular/scripts/Common",
					"DayZular/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"DayZular/scripts/Common",
					"DayZular/scripts/5_Mission"
				};
			};
		};
	};
};
