/*
*	DCI Variable Default Implementation
*
*	(C) 2002-2003 Bayer AG Leverkusen, BTS-PT-AS-CS
*
*	Project: Data Mining / DCI
*	Author:  Dirk ter Huerne (ZTMEY)
*
*   THIS SOFTWARE IS PROVIDED BY THE BAYER TECHNOLOGY SERVICES GMBH "AS IS"
*	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*	ARE DISCLAIMED.
*	IN NO EVENT SHALL THE BAYER AG NOR THE AUTHOR OF THIS CODE BE LIABLE FOR
*	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
*	LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
*	OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
*	SUCH DAMAGE.
*/

#pragma warning(disable:4530)

#include "DCI/Helpers.h"
#include "DCI/Variable.h"
#include "DCI/Error.h"
#include "DCI/Utilities.h"

#include <ymath.h>

namespace DCI {

Variable::Variable(Table *table, FieldDef *fieldDef, IVariable *variableToClone) 
	: m_Table(table), m_Values(0), m_FieldDef(fieldDef) 
{
	assert(table);
	assert(fieldDef);
	assert(!fieldDef->m_Column);
	fieldDef->m_Column = this;
	InternalSetDataType(fieldDef->m_DataType);
	if (variableToClone) {
		/*b=*/ SetValues(variableToClone->GetValues());
    } else {
        if (table->RecordsGetCount()) InternalReDim();
    }
}

Variable::~Variable() {
	assert(m_Values);
	delete m_Values;
}

void Variable::AddRef() {
	m_RefCount++;
    if (m_Table) m_Table->AddRef();
}

void Variable::Release() {
	m_RefCount--;
    if (m_Table) m_Table->Release(); else if (!m_RefCount) delete this;
}

ITableHandle Variable::GetTable() const {
	return m_Table;
}

IFieldDefHandle Variable::GetFieldDef() const {
	return m_FieldDef;
}

Vector Variable::GetValues() const {
	return *m_Values;
}

StringVector Variable::GetValuesAsString() const {
    switch (m_FieldDef->GetDataType()) {
    case DT_BYTE: 
        {
            StringVector sv;
            size_t       len = m_Values->Len();
            size_t       i;
		    ByteVector  *byteValues = (ByteVector *)m_Values;
            Byte         val;

            sv.ReDim(len);
            for (i=0; i<len; i++) {
                val = (*byteValues)[i];
                sv[i] = ToString(val).c_str();
            }
            return sv;
        }
    case DT_INT: 
        {
            StringVector sv;
            size_t       len = m_Values->Len();
            size_t       i;
		    IntVector   *intValues = (IntVector *)m_Values;
            Int          val;

            sv.ReDim(len);
            for (i=0; i<len; i++) {
                val = (*intValues)[i];
                sv[i] = ToString(val).c_str();
            }
            return sv;
        }
    case DT_DOUBLE:
        {
            StringVector  sv;
            size_t        len = m_Values->Len();
            size_t        i;
		    DoubleVector *dblValues = (DoubleVector *)m_Values;
            Double        val;

            sv.ReDim(len);
            for (i=0; i<len; i++) {
                val = (*dblValues)[i];
                sv[i] = ToString(val).c_str();
            }
            return sv;
        }
    case DT_STRING:
        return *(StringVector *)m_Values;
    case DT_DATETIME:
        {
            StringVector sv;
            Utilities::DateTimeVectorToStringVector(*(DoubleVector *)m_Values, sv, String()); // TODO -- format?
            return sv;
        }
    case DT_ENUMERATION:
        {
            StringVector sv;
            Utilities::EnumVectorToStringVector(*(IntVector *)m_Values, sv, m_FieldDef->GetAllowedValues());
            return sv;
        }
    default: 
        break;
    }
    return StringVector();
}

Bool Variable::SetValues(const Vector &newValues) {
	// check if the data type and the access rights are correct
    if (!m_FieldDef) return false;
    if (m_FieldDef->GetDataType() == DT_ENUMERATION) {
        // special case: enumeration
        if ((newValues.GetDataType() != DT_INT) && (newValues.GetDataType() != DT_STRING)) return false;
    } else {
	    if (m_Values->GetDataType() != newValues.GetDataType()) return false;
    }

	// set the new value
    if ((m_FieldDef->GetDataType() == DT_ENUMERATION) && (newValues.GetDataType() == DT_STRING)) {
        // special case: enumeration
        StringVector sv = newValues;
        StringVector av = m_FieldDef->m_AllowedValues;
        Utilities::StringVectorToEnumVectorEx(sv, *(IntVector *)m_Values, av);
        if (m_FieldDef->m_AllowedValues.Len() != av.Len()) {
            m_FieldDef->m_AllowedValues = av;
            m_FieldDef->m_MaxValue = (Int)(av.Len() - 1);
        }
    } else {
    	*m_Values = newValues;
    }

	// redim the vector value's length and initialize missing fields
	// with their default value
	return InternalReDim();
}

Value Variable::GetValue(UInt recIdx) const {
	if ((recIdx < 1) || (recIdx > m_Values->Len())) return Value();
	
	switch (m_Values->GetDataType()) {
	case DT_VOID:
		return Value();
	case DT_BYTE: {
		    ByteVector *byteValues = (ByteVector *)m_Values;
		    return (*byteValues)[recIdx-1];
	    }
	case DT_INT: {
		    IntVector *lngValues = (IntVector *)m_Values;
		    return (*lngValues)[recIdx-1];
	    }
	case DT_DOUBLE: {
		    DoubleVector *dblValues = (DoubleVector *)m_Values;
		    return (*dblValues)[recIdx-1];
	    }
	case DT_STRING: {
		    StringVector *strValues = (StringVector *)m_Values;
		    return (*strValues)[recIdx-1];
	    }
	default:
		// should never happen
		assert(false);
		return Value();
	}
}

String Variable::GetValueAsString(UInt recIdx) const {
	if ((recIdx < 1) || (recIdx > m_Values->Len())) return String();

    switch (m_FieldDef->GetDataType()) {
    case DT_BYTE: {
            ByteVector *byteValues = (ByteVector *)m_Values;
            return ToString( (*byteValues)[recIdx-1] ).c_str();
        }
    case DT_INT: {
		    IntVector *lngValues = (IntVector *)m_Values;
		    return ToString( (*lngValues)[recIdx-1] ).c_str();
        }
    case DT_DOUBLE: {
		    DoubleVector *dblValues = (DoubleVector *)m_Values;
		    return ToString( (*dblValues)[recIdx-1] ).c_str();
	    }
    case DT_STRING:
        return (*(StringVector *)m_Values)[recIdx-1];
    case DT_DATETIME: {
            String s;
            Utilities::DateTimeToString((*(DoubleVector *)m_Values)[recIdx-1], s, ""); // TODO -- format
            return s;
        }
    case DT_ENUMERATION: {
            String s;
            Utilities::EnumToString((*(IntVector *)m_Values)[recIdx-1], s, m_FieldDef->GetAllowedValues());
            return s;
        }
    default: 
        return String();
    }
}

Bool Variable::SetValue(UInt recIdx, const Value &newValue) {
    // check preconditions
	if(!InternalCanSetValue(recIdx, newValue)) return false;

	// set the new value
	switch (newValue.GetDataType()) {
	case DT_VOID:
		break;
	case DT_BYTE: 
        {
		    ByteVector *byteValues = (ByteVector *)m_Values;
		    (*byteValues)[recIdx-1] = (const Byte)newValue;
	    }
		break;
	case DT_INT: 
        {
		    IntVector *lngValues = (IntVector *)m_Values;
		    (*lngValues)[recIdx-1] = (const Int)newValue;
	    }
		break;
	case DT_DOUBLE: 
        {
		    DoubleVector *dblValues = (DoubleVector *)m_Values;
		    (*dblValues)[recIdx-1] = (const Double)newValue;
	    }
		break;
	case DT_STRING: 
        if (m_FieldDef->GetDataType() == DT_ENUMERATION) {
            // special case: enumeration
            Int e;
            StringVector av = m_FieldDef->m_AllowedValues;
            Utilities::StringToEnumEx((const String)newValue, e, av);
		    IntVector *lngValues = (IntVector *)m_Values;
		    (*lngValues)[recIdx-1] = e;
            if (m_FieldDef->m_AllowedValues.Len() != av.Len()) {
                m_FieldDef->m_AllowedValues = av;
                m_FieldDef->m_MaxValue = (Int)(av.Len() - 1);
            }
        } else {
		    StringVector *strValues = (StringVector *)m_Values;
		    (*strValues)[recIdx-1] = (const String)newValue;
	    }
		break;
	default:
		// should never happen
		assert(!"This should never happen");
		return false;
	}
	return true;
}

UInt Variable::GetLength() const {
	assert(m_Values);
	return m_Values->Len();
}

Bool Variable::InternalReDim() {
	assert(m_Values);

	// if a table is not associated or it is record based, just return
    if (!m_Table) return true;
	if (!m_Table->GetRecordBased()) return true;

	UInt oldLen = m_Values->Len();
	UInt newLen = m_Table->RecordsGetCount();

    // redimension the value vector initializing it with the default value
	if (!m_Values->ReDim(newLen, m_FieldDef->GetDefaultValue())) return false;
	return true;
}

Bool Variable::InternalRemove(UInt Index) {
    assert(m_Values);

	// if a table is not associated or it is record based, just return
    if (!m_Table) return true;
	if (!m_Table->GetRecordBased()) return true;

    // remove the (Index)th element from the value vector
	switch (m_Values->GetDataType()) {
	case DT_VOID:
		break;
	case DT_BYTE:
		if (!((ByteVector *)m_Values)->Remove(Index - 1)) return false;
		break;
	case DT_INT:
		if (!((IntVector *)m_Values)->Remove(Index - 1)) return false;
		break;
	case DT_DOUBLE: 
		if (!((DoubleVector *)m_Values)->Remove(Index - 1)) return false;
		break;
	case DT_STRING:
		if (!((StringVector *)m_Values)->Remove(Index - 1)) return false;
		break;
	default:
		// should never happen
		assert(false);
		return false;
	}
	return true;
}

void Variable::InternalSetDataType(DataType newDataType) {
	// convert a semantic data type to a technical data type
	switch (newDataType) {
	case DT_ENUMERATION:
		newDataType = DT_INT;
        break;
	case DT_DATETIME:
		newDataType = DT_DOUBLE;
        break;
	default:
		break;
	}

	// exit if data type does not change
	if (m_Values) {
		if (m_Values->GetDataType() == newDataType) return;
	}

	// delete old vector
	if (m_Values) delete m_Values;

	// create new vector
	switch (newDataType) {
	case DT_VOID:
		m_Values = new Vector();
		break;
	case DT_BYTE:
		m_Values = new ByteVector();
		break;
	case DT_INT:
		m_Values = new IntVector();
		break;
	case DT_DOUBLE:
		m_Values = new DoubleVector();
		break;
	case DT_STRING:
		m_Values = new StringVector();
		break;
	default:
		assert(!"Unhandled data type");
		m_Values = new Vector();
		return;
	}
	assert(m_Values);
}

Bool Variable::InternalCanSetValue(UInt recIdx, const Value &newValue) {
	assert(m_Values);

	// check if data type, index and access rights are correct
    if (!m_FieldDef) return false;
    if (m_FieldDef->GetDataType() == DT_ENUMERATION) {
        // special case: enumeration
        if ((newValue.GetDataType() != DT_INT) && (newValue.GetDataType() != DT_STRING)) return false;
    } else {
	    if (m_Values->GetDataType() != newValue.GetDataType()) return false;
    }
	if ((recIdx < 1) || (recIdx > m_Values->Len())) return false;
	return true;
}

// Load/Save variable from/to file

Variable::Variable(Table *table, FieldDef *fieldDef, FILE *fp)
	: m_Table(table), m_Values(0), m_FieldDef(fieldDef) 
{
	assert(table);
	assert(fieldDef);
	assert(!fieldDef->m_Column);
	fieldDef->m_Column = this;
    m_Values = new Vector(fp);
}

Bool Variable::SaveToBinaryFile(FILE *fp) const {
    assert(m_Values);
    return m_Values->SaveToBinaryFile(fp);
}

} /* namespace DCI */
