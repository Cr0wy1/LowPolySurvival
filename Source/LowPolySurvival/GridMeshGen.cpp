// Fill out your copyright notice in the Description page of Project Settings.


#include "GridMeshGen.h"
#include "RunnableThread.h"


void UGridMeshGen::EnsureCompletion(){
	
	if (task) {
		task->EnsureCompletion();
		if (task->Cancel()) {
			delete task;
			task = NULL;
		}
	}

	return;
	//if (gridMeshGenThread) {
	//	gridMeshGenThread->EnsureCompletion();
	//	delete gridMeshGenThread; 
	//	gridMeshGenThread = nullptr;
	//}
}

FMeshGenData* UGridMeshGen::CreateMesh(const FBlockGrid &blockGrid, const FGridMeshParams &_params) {
	gridSize = blockGrid.dims;
	params = _params;

	//UE_LOG(LogTemp, Warning, TEXT("UGridMeshGen: Createmesh"));


	meshGenData.bIsReady = false; 
	task = new FAsyncTask<GridMeshGenTask>(this, &blockGrid, false);
	task->StartBackgroundTask();
	
	//gridMeshGenThread = new GridMeshGenThread(this, &blockGrid, false);
	//gridMeshGenThread = GridMeshGenThread::JoyInt(this, &blockGrid, false);

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

void UGridMeshGen::DoTaskWork(bool bUpdateOnly, const FBlockGrid &blockGrid){

	meshGenData.bIsReady = true;

	FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&](){
		OnFinishMeshGen.Broadcast();
	}, TStatId(), NULL, ENamedThreads::GameThread);

	
}



//##########################################################
//################### ProcMeshTask #########################

GridMeshGenTask::GridMeshGenTask(UGridMeshGen * _gridMeshGen, const FBlockGrid* _blockGrid, bool _bUpdateOnly) : blockGrid(_blockGrid){
	gridMeshGen = _gridMeshGen;
	bUpdateOnly = _bUpdateOnly;
}

GridMeshGenTask::~GridMeshGenTask() {
	//UE_LOG(LogTemp, Warning, TEXT("ProcMeshTask: Destructor!"));
}

void GridMeshGenTask::DoWork() {
	if (blockGrid) {
		gridMeshGen->DoTaskWork(bUpdateOnly, *blockGrid);
	}
	
}


//####################### GridMeshGenThread #############################

GridMeshGenThread* GridMeshGenThread::runnable = nullptr;

GridMeshGenThread::GridMeshGenThread(UGridMeshGen * _gridMeshGen, const FBlockGrid * _blockGrid, bool _bUpdateOnly) : blockGrid(_blockGrid){

	gridMeshGen = _gridMeshGen;
	bUpdateOnly = _bUpdateOnly;
	
	//Thread = FRunnableThread::Create(this, TEXT("GridMeshThread"), 0, EThreadPriority::TPri_AboveNormal);
}

GridMeshGenThread::~GridMeshGenThread(){

	if (Thread) {
		delete Thread;
		Thread = nullptr;
	}
}

GridMeshGenThread * GridMeshGenThread::JoyInt(UGridMeshGen * _gridMeshGen, const FBlockGrid * _blockGrid, bool _bUpdateOnly){

	if (!runnable) {
		runnable = new GridMeshGenThread(_gridMeshGen, _blockGrid, _bUpdateOnly);
	}
	
	runnable->gridGens.Enqueue(_gridMeshGen);
	//runnable->gridGens.Add(_gridMeshGen);

	return runnable;
}

bool GridMeshGenThread::Init(){

	//UE_LOG(LogTemp, Warning, TEXT("Thread: Init"));


	return true;
}

uint32 GridMeshGenThread::Run(){

	//FPlatformProcess::Sleep(0.03);

	//gridMeshGen->DoTaskWork(bUpdateOnly, *blockGrid);
	//while (!bKill) {
	
		//GridMeshGen* gridMeshGen;
		
		//if (gridGens.Dequeue(gridMeshGen)) {
			//gridMeshGen->DoTaskWork(bUpdateOnly, *blockGrid);
		//}
		
	//}
	//gridMeshGen->DoTaskWork(bUpdateOnly, *blockGrid);
	//UE_LOG(LogTemp, Warning, TEXT("Thread: Run"));

	return 0;
}

void GridMeshGenThread::Stop(){
	
	bKill = true;

	//UE_LOG(LogTemp, Warning, TEXT("Thread: Stop"));

}

void GridMeshGenThread::EnsureCompletion(){
	Stop();

	if (Thread) {
		Thread->WaitForCompletion();
	}
}
