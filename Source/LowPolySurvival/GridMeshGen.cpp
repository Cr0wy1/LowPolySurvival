// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMeshGen.h"


FMeshGenData* UGridMeshGen::CreateMesh(const FBlockGrid &blockGrid, const FGridMeshParams &_params) {
	gridSize = blockGrid.dims;
	params = _params;

	//GridMeshGenTask task(this, false);
	//task.DoWork();
	//(new FAutoDeleteAsyncTask<GridMeshGenTask>(this, false))->StartBackgroundTask();

	return &meshGenData;
}


FMeshGenData* UGridMeshGen::UpdateMesh(const FBlockGrid & blockGrid, const FIntVector & blockLoc, const FGridMeshParams &_params){
	gridSize = blockGrid.dims;
	params = _params;

	//GridMeshGenTask task(this, true);
	//task.DoWork();
	//(new FAutoDeleteAsyncTask<GridMeshGenTask>(this, true))->StartBackgroundTask();

	return &meshGenData;
}

bool UGridMeshGen::IsReady() const{

	return meshGenData.bIsReady;
}

void UGridMeshGen::DoTaskWork(bool bUpdateOnly){

}



//##########################################################
//################### ProcMeshTask #########################

GridMeshGenTask::GridMeshGenTask(UGridMeshGen * _gridMeshGen, bool _bUpdateOnly){
	gridMeshGen = _gridMeshGen;
	bUpdateOnly = _bUpdateOnly;
}

GridMeshGenTask::~GridMeshGenTask() {
	//UE_LOG(LogTemp, Warning, TEXT("ProcMeshTask: Destructor!"));
}

void GridMeshGenTask::DoWork() {
	gridMeshGen->DoTaskWork(bUpdateOnly);


}

