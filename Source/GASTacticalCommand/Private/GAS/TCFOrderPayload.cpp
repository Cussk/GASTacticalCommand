//Copyright Kyle Cuss and Cuss Programming 2026.

#include "GAS/TCFOrderPayload.h"

#include "Data/TCFOrderDefinition.h"

void UTCFOrderPayload::Initialize(const FTCFSquadOrderRequest& InRequest, UTCFOrderDefinition* InOrderDefinition)
{
	Request = InRequest;
	OrderDefinition = InOrderDefinition;
}

const FTCFSquadOrderRequest& UTCFOrderPayload::GetRequest() const
{
	return Request;
}

UTCFOrderDefinition* UTCFOrderPayload::GetOrderDefinition() const
{
	return OrderDefinition;
}

bool UTCFOrderPayload::IsValidPayload() const
{
	return Request.IsValidRequest() && IsValid(OrderDefinition);
}