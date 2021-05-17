// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class SpaceRocksTarget : TargetRules
{
	public SpaceRocksTarget(TargetInfo Target) : base (Target)
	{
		Type = TargetType.Game;
		ExtraModuleNames.Add("SpaceRocks");
	}

	//
	// TargetRules interface.
	//
	
}
