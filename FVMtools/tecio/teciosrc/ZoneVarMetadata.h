 #pragma once
#include "SzlFileLoader.h"
namespace tecplot { namespace ___3933 { class ___37; class ItemSetIterator; struct ___4707 { ___3816 m_zoneNames; ___473 m_zoneTypes; UInt8Array m_zoneFaceNeighborModes; ___1105 m_zoneSolutionTimes; UInt32Array m_zoneStrandIDs; ___4635 m_zoneParentZones; UInt64Array m_zoneIMaxOrNumNodes; UInt64Array m_zoneJMaxOrNumCells; UInt64Array m_zoneKMaxOrNumCorners; ___4635 m_zoneShareConnectivityWithZone; ___3816 m_varNames; VarZoneMinMaxArray m_vzMinMaxes; ___4391 m_vzValueLocations; ___4382 m_vzFieldDataTypes; ___4385 m_vzIsPassive; ___4388 m_vzShareVarWithZone; ___4707( ___37&      ___36, ItemSetIterator& varIter, ItemSetIterator& zoneIter); }; }}