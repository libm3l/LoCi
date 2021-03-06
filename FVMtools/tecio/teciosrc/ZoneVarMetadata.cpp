#include "ZoneVarMetadata.h"
#include "ThirdPartyHeadersBegin.h"
#include <cctype>
#include <new>
#include <sstream>
#include <boost/assign.hpp>
#include <boost/static_assert.hpp>
#include "ThirdPartyHeadersEnd.h"
#include "AltTecUtil.h"
#include "fileStuff.h"
#include "ItemSetIterator.h"
#include "zoneUtil.h"
namespace tecplot { namespace ___3933 { namespace { ___4636 varGetPrevSharedZone( ___4636           zone, ___4636           ___341, ___4352            ___4336, ___37&           ___36, ___4635 const& zoneShareConnectivityWithZone) { ___4636 prevSharedZone = ___36.___914(NULL, zone + 1, ___4336 + 1) - 1; if (prevSharedZone != NOT_SHARED) { if (___36.___4620(zone + 1) == ___4704 && ___36.___4620((___4636)prevSharedZone + 1) == ___4704) { ___1844 ___4632; ___1844 sharedZoneIJK; ___36.___4615((___4636)(zone + 1), ___4632); ___36.___4615((___4636)(prevSharedZone + 1), sharedZoneIJK); if (___4632 != sharedZoneIJK) prevSharedZone = NOT_SHARED; } else if (zoneShareConnectivityWithZone[zone - ___341] == NOT_SHARED) { prevSharedZone = NOT_SHARED; } } return prevSharedZone; } } ___4707::___4707( ___37&      ___36, ItemSetIterator& varIter, ItemSetIterator& zoneIter) { BOOST_STATIC_ASSERT(static_cast<size_t>(END_ValueLocation_e) <= uint8_t(-1)); BOOST_STATIC_ASSERT(static_cast<size_t>(END_FieldDataType_e) <= uint8_t(-1)); if (!___36.___896()) return; ___4352 const numVarsToWrite = varIter.___2812(); ___4636 const numZonesToWrite = zoneIter.___2812(); if (!m_zoneNames.alloc(numZonesToWrite) || !m_zoneTypes.alloc(numZonesToWrite) || !m_zoneFaceNeighborModes.alloc(numZonesToWrite) || !m_zoneSolutionTimes.alloc(numZonesToWrite) || !m_zoneStrandIDs.alloc(numZonesToWrite) || !m_zoneParentZones.alloc(numZonesToWrite) || !m_zoneIMaxOrNumNodes.alloc(numZonesToWrite) || !m_zoneJMaxOrNumCells.alloc(numZonesToWrite) || !m_zoneKMaxOrNumCorners.alloc(numZonesToWrite) || !m_zoneShareConnectivityWithZone.alloc(numZonesToWrite) || !m_varNames.alloc(numVarsToWrite) || !___3356(m_vzMinMaxes, numVarsToWrite, numZonesToWrite) || !___3356(m_vzValueLocations, numVarsToWrite, numZonesToWrite) || !___3356(m_vzFieldDataTypes, numVarsToWrite, numZonesToWrite) || !___3356(m_vzIsPassive, numVarsToWrite, numZonesToWrite) || !___3356(m_vzShareVarWithZone, numVarsToWrite, numZonesToWrite)) { throw std::bad_alloc(); } std::map<ZoneType_e, char> zoneTypeMap = boost::assign::map_list_of (___4704, ___4705) (___4702, ZONETYPE_FETRIANGLE_CHAR) (___4700, ZONETYPE_FEQUAD_CHAR) (___4701, ZONETYPE_FETETRA_CHAR) (___4695, ZONETYPE_FEBRICK_CHAR) (___4696, ZONETYPE_FELINESEG_CHAR) (___4698, ZONETYPE_FEPOLYGON_CHAR) (___4699, ZONETYPE_FEPOLYHEDRON_CHAR) (___4697, ZONETYPE_FEMIXEDVOLUME_CHAR); zoneIter.reset(); ___4636 const ___341 = zoneIter.baseItem(); while (zoneIter.hasNext()) { ___4636 const ___904 = zoneIter.next(); ___4636 const fileZone = ___904 - ___341; if (!___36.___4638(___904 + 1)) continue; char *zoneName = NULL; if (___36.___4616(___904 + 1, &zoneName)) { m_zoneNames[fileZone] = zoneName; ___36.___3817(&zoneName); } else { throw std::bad_alloc(); } m_zoneFaceNeighborModes[fileZone] = static_cast<uint8_t>(___36.___836(___904 + 1)); ___1844 ___4632; ___36.___4615(___904 + 1, ___4632); ZoneType_e const ___4692 = ___36.___4620(___904 + 1); m_zoneTypes[fileZone] = zoneTypeMap[___4692]; if (___3895(___4692, ___4632)) m_zoneTypes[fileZone] = static_cast<char>(std::toupper(m_zoneTypes[fileZone])); m_zoneSolutionTimes[fileZone] = ___36.___4618(___904 + 1); m_zoneStrandIDs[fileZone] = ___36.___4619(___904 + 1); ___4636 ___2974 = ___36.___4617(___904 + 1) - 1; ___478(___2974 == -1 || ___2974 >= ___341); m_zoneParentZones[fileZone] = ___2974 == -1 ? ___2974 : ___2974 - ___341; if (___4692 == ___4704) { m_zoneIMaxOrNumNodes[fileZone]   = ___4632.i(); m_zoneJMaxOrNumCells[fileZone]   = ___4632.___2105(); m_zoneKMaxOrNumCorners[fileZone] = ___4632.___2134(); } else { m_zoneIMaxOrNumNodes[fileZone] = ___4632.___1670(); m_zoneJMaxOrNumCells[fileZone] = ___4632.___1668(); if (___3158(___4692)) m_zoneKMaxOrNumCorners[fileZone] = ___4632.___2134(); else m_zoneKMaxOrNumCorners[fileZone] = ___4632.___1669(); } ___4636 const prevSharedZone = ___36.___544(NULL, ___904 + 1) - 1; if (prevSharedZone == NOT_SHARED) m_zoneShareConnectivityWithZone[fileZone] = NOT_SHARED; else if (prevSharedZone < ___341) m_zoneShareConnectivityWithZone[fileZone] = NOT_SHARED; else if (m_zoneShareConnectivityWithZone[prevSharedZone - ___341] == NOT_SHARED) m_zoneShareConnectivityWithZone[fileZone] = prevSharedZone - ___341; else m_zoneShareConnectivityWithZone[fileZone] = m_zoneShareConnectivityWithZone[prevSharedZone - ___341]; ___478(m_zoneShareConnectivityWithZone[fileZone] == NOT_SHARED || m_zoneShareConnectivityWithZone[m_zoneShareConnectivityWithZone[fileZone]] == NOT_SHARED);
} varIter.reset(); ___4352 const baseVar = varIter.baseItem(); while (varIter.hasNext()) { ___4352 const datasetVar = varIter.next(); ___4352 const fileVar = datasetVar - baseVar; char* ___4362 = NULL; if (___36.___4344(datasetVar+1, &___4362)) { m_varNames[fileVar] = ___4362; ___36.___3817(&___4362); } else { throw std::bad_alloc(); } } varIter.reset(); while (varIter.hasNext()) { ___4352 const datasetVar = varIter.next(); ___4352 const fileVar = datasetVar - baseVar; zoneIter.reset(); ___4636 const ___341 = zoneIter.baseItem(); while (zoneIter.hasNext()) { ___4636 const ___904 = zoneIter.next(); ___4636 const fileZone = ___904 - ___341; if (!___36.___4638(___904 + 1)) continue; double minValue; double maxValue; ___36.___913(___904 + 1, datasetVar + 1, &minValue, &maxValue); m_vzMinMaxes[fileVar][fileZone] = ___2479(minValue, maxValue); ValueLocation_e ___4326 = ___36.___910(___904 + 1, datasetVar + 1); m_vzValueLocations[fileVar][fileZone] = static_cast<uint8_t>(___4326); FieldDataType_e ___1363 = ___36.___923(___904+1,datasetVar+1); m_vzFieldDataTypes[fileVar][fileZone] = static_cast<uint8_t>(___1363); ___372 varIsPassive = ___36.___926(___904 + 1, datasetVar + 1); m_vzIsPassive[fileVar][fileZone] = (varIsPassive ? 1 : 0); ___4636 const prevSharedZone = varGetPrevSharedZone( ___904, ___341, datasetVar, ___36, m_zoneShareConnectivityWithZone); if (prevSharedZone == NOT_SHARED) m_vzShareVarWithZone[fileVar][fileZone] = NOT_SHARED; else if (prevSharedZone < ___341) m_vzShareVarWithZone[fileVar][fileZone] = NOT_SHARED; else if (m_vzShareVarWithZone[fileVar][prevSharedZone - ___341] == NOT_SHARED) m_vzShareVarWithZone[fileVar][fileZone] = prevSharedZone - ___341; else m_vzShareVarWithZone[fileVar][fileZone] = m_vzShareVarWithZone[fileVar][prevSharedZone - ___341]; ___478(m_vzShareVarWithZone[fileVar][fileZone] == NOT_SHARED || m_vzShareVarWithZone[fileVar][ m_vzShareVarWithZone[fileVar][fileZone]] == NOT_SHARED); } } } }}
