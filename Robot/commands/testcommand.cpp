#include "testcommand.h"

void TestCommand::Init()
{

}

void TestCommand::Run()
{

}

bool TestCommand::EndCondition()
{
    return io->ButtonBoardGetButton(io->RIGHT);
}

void TestCommand::Finish()
{

}

bool TestCommand::FailedCondition()
{
    return false;
}

void TestCommand::Failure()
{

}

void TestCommand::PrintStatus()
{
    lcd->WriteLine("Exit: Right Button");
    lcd->Write("CdS: ");
    lcd->WriteLine(io->cds_cell->Value());
    lcd->Write("Opto: ");
    lcd->WriteLine(io->optosensor->Value());
    lcd->Write("Left switch: ");
    lcd->WriteLine(!io->left_switch->Value());
    lcd->Write("Right switch: ");
    lcd->WriteLine(!io->right_switch->Value());
    lcd->Write("Arm switch: ");
    lcd->WriteLine(!io->arm_switch->Value());
    lcd->Write("Left encoder: ");
    lcd->WriteLine(io->left_encoder->Counts());
    lcd->Write("Right encoder: ");
    lcd->WriteLine(io->right_encoder->Counts());
}

