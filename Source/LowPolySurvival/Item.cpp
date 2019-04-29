// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"
#include "Components/StaticMeshComponent.h"
#include "Buildings.h"


// Sets default values
UItem::UItem(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	

}



FItemInfo UItem::GetItemInfo() const{

	return itemInfo;
}

