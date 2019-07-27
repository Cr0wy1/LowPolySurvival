// Fill out your copyright notice in the Description page of Project Settings.

#include "GameStructs.h"

bool FBlock::ApplyDamage(int32 damageAmount){
	durability -= damageAmount;

	return durability < 1;
}

void FBlock::SetResource(FResource * _resource){
	resource = _resource;
	durability = _resource->hardness;
	data.blockId = _resource->id;
	data.color = _resource->color.ToFColor(true);
}

bool FBlock::IsValid() const{
	return resource;
}
