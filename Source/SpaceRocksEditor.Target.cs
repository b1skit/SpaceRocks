// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class SpaceRocksEditorTarget : TargetRules
{
	public SpaceRocksEditorTarget(TargetInfo Target) : base (Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.Add("SpaceRocks");
	}

	//
	// TargetRules interface.
	//

	
}
