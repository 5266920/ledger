/*
 * Copyright (c) 2003-2007, John Wiegley.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of New Artisans LLC nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "value.h"
#include "node.h"

namespace ledger {

intrusive_ptr<value_t::storage_t> value_t::true_value;
intrusive_ptr<value_t::storage_t> value_t::false_value;

void value_t::storage_t::destroy()
{
  switch (type) {
  case AMOUNT:
    ((amount_t *)data)->~amount_t();
    break;
  case BALANCE:
    checked_delete(*(balance_t **)data);
    break;
  case BALANCE_PAIR:
    checked_delete(*(balance_pair_t **)data);
    break;
  case STRING:
    ((string *)data)->~string();
    break;
  case SEQUENCE:
    checked_delete(*(sequence_t **)data);
    break;
  case POINTER:
    ((boost::any *)data)->~any();
    break;

  default:
    break;
  }
  type = VOID;
}

void value_t::initialize()
{
#if 0
  LOGGER("value.initialize");
#endif

  true_value = new storage_t;
  true_value->type = BOOLEAN;
  *(bool *) true_value->data = true;

  false_value = new storage_t;
  false_value->type = BOOLEAN;
  *(bool *) false_value->data = false;

  BOOST_STATIC_ASSERT(sizeof(amount_t) >= sizeof(bool));
  BOOST_STATIC_ASSERT(sizeof(amount_t) >= sizeof(moment_t));
  BOOST_STATIC_ASSERT(sizeof(amount_t) >= sizeof(long));
  BOOST_STATIC_ASSERT(sizeof(amount_t) >= sizeof(amount_t));
  BOOST_STATIC_ASSERT(sizeof(amount_t) >= sizeof(balance_t *));
  BOOST_STATIC_ASSERT(sizeof(amount_t) >= sizeof(balance_pair_t *));
  BOOST_STATIC_ASSERT(sizeof(amount_t) >= sizeof(string));
  BOOST_STATIC_ASSERT(sizeof(amount_t) >= sizeof(sequence_t *));
  BOOST_STATIC_ASSERT(sizeof(amount_t) >= sizeof(xml::node_t *));
  BOOST_STATIC_ASSERT(sizeof(amount_t) >= sizeof(boost::any));

#if 0
  DEBUG_(std::setw(3) << std::right << sizeof(bool)
	 << "  sizeof(bool)");
  DEBUG_(std::setw(3) << std::right << sizeof(moment_t)
	 << "  sizeof(moment_t)");
  DEBUG_(std::setw(3) << std::right << sizeof(long)
	 << "  sizeof(long)");
  DEBUG_(std::setw(3) << std::right << sizeof(amount_t)
	 << "  sizeof(amount_t)");
  DEBUG_(std::setw(3) << std::right << sizeof(balance_t *)
	 << "  sizeof(balance_t *)");
  DEBUG_(std::setw(3) << std::right << sizeof(balance_pair_t *)
	 << "  sizeof(balance_pair_t *)");
  DEBUG_(std::setw(3) << std::right << sizeof(string)
	 << "  sizeof(string)");
  DEBUG_(std::setw(3) << std::right << sizeof(sequence_t *)
	 << "  sizeof(sequence_t *)");
  DEBUG_(std::setw(3) << std::right << sizeof(boost::any)
	 << "  sizeof(boost::any)");
#endif
}

void value_t::shutdown()
{
  true_value  = intrusive_ptr<storage_t>();
  false_value = intrusive_ptr<storage_t>();
}

value_t::operator bool() const
{
  switch (type()) {
  case BOOLEAN:
    return as_boolean();
  case INTEGER:
    return as_long();
  case DATETIME:
    return is_valid_moment(as_datetime());
  case AMOUNT:
    return as_amount();
  case BALANCE:
    return as_balance();
  case BALANCE_PAIR:
    return as_balance_pair();
  case STRING:
    return ! as_string().empty();
  case SEQUENCE:
    return ! as_sequence().empty();
  case XML_NODE:
    return as_xml_node()->to_value();
  case POINTER:
    return ! as_any_pointer().empty();
  default:
    assert(false);
    break;
  }
  assert(false);
  return 0;
}

bool value_t::to_boolean() const
{
  if (is_boolean()) {
    return as_boolean();
  } else {
    value_t temp(*this);
    temp.in_place_cast(BOOLEAN);
    return temp.as_boolean();
  }
}

long value_t::to_long() const
{
  if (is_long()) {
    return as_long();
  } else {
    value_t temp(*this);
    temp.in_place_cast(INTEGER);
    return temp.as_long();
  }
}

moment_t value_t::to_datetime() const
{
  if (is_datetime()) {
    return as_datetime();
  } else {
    value_t temp(*this);
    temp.in_place_cast(DATETIME);
    return temp.as_datetime();
  }
}

amount_t value_t::to_amount() const
{
  if (is_amount()) {
    return as_amount();
  } else {
    value_t temp(*this);
    temp.in_place_cast(AMOUNT);
    return temp.as_amount();
  }
}

balance_t value_t::to_balance() const
{
  if (is_balance()) {
    return as_balance();
  } else {
    value_t temp(*this);
    temp.in_place_cast(BALANCE);
    return temp.as_balance();
  }
}

balance_pair_t value_t::to_balance_pair() const
{
  if (is_balance_pair()) {
    return as_balance_pair();
  } else {
    value_t temp(*this);
    temp.in_place_cast(BALANCE_PAIR);
    return temp.as_balance_pair();
  }
}

string value_t::to_string() const
{
  if (is_string()) {
    return as_string();
  } else {
    value_t temp(*this);
    temp.in_place_cast(STRING);
    return temp.as_string();
  }
}

value_t::sequence_t value_t::to_sequence() const
{
  if (is_sequence()) {
    return as_sequence();
  } else {
    value_t temp(*this);
    temp.in_place_cast(SEQUENCE);
    return temp.as_sequence();
  }
}


void value_t::in_place_simplify()
{
  LOGGER("amounts.values.simplify");

  if (is_realzero()) {
    DEBUG_("Zeroing type " << type());
    set_long(0L);
    return;
  }

  if (is_balance_pair() &&
      (! as_balance_pair().cost || as_balance_pair().cost->is_realzero())) {
    DEBUG_("Reducing balance pair to balance");
    in_place_cast(BALANCE);
  }

  if (is_balance() && as_balance().amounts.size() == 1) {
    DEBUG_("Reducing balance to amount");
    in_place_cast(AMOUNT);
  }

#if 0
  if (is_amount() && ! as_amount().has_commodity() &&
      as_amount().fits_in_long()) {
    DEBUG_("Reducing amount to integer");
    in_place_cast(INTEGER);
  }
#endif
}

value_t& value_t::operator+=(const value_t& val)
{
  if (is_string()) {
    if (val.is_string())
      as_string_lval() += val.as_string();
    else
      as_string_lval() += val.to_string();
    return *this;
  }
  else if (is_sequence()) {
    if (val.is_sequence()) {
      sequence_t& seq(as_sequence_lval());
      seq.insert(seq.end(), val.as_sequence().begin(),
		 val.as_sequence().end());
    } else {
      as_sequence_lval().push_back(val);
    }
    return *this;
  }

  if (val.is_xml_node()) // recurse
    return *this += val.as_xml_node()->to_value();

  switch (type()) {
  case DATETIME:
    switch (val.type()) {
    case INTEGER:
      as_datetime_lval() += date_duration(val.as_long());
      return *this;
    case AMOUNT:
      as_datetime_lval() += date_duration(val.as_amount().to_long());
      return *this;
    default:
      break;
    }
    break;

  case INTEGER:
    switch (val.type()) {
    case INTEGER:
      as_long_lval() += val.as_long();
      return *this;
    case AMOUNT:
      in_place_cast(AMOUNT);
      as_amount_lval() += val.as_amount();
      return *this;
    case BALANCE:
      in_place_cast(BALANCE);
      as_balance_lval() += val.as_balance();
      return *this;
    case BALANCE_PAIR:
      in_place_cast(BALANCE_PAIR);
      as_balance_pair_lval() += val.as_balance_pair();
      return *this;
    default:
      break;
    }
    break;

  case AMOUNT:
    switch (val.type()) {
    case INTEGER:
      if (as_amount().has_commodity()) {
	in_place_cast(BALANCE);
	return *this += val;
      } else {
	as_amount_lval() += val.as_long();
	return *this;
      }
      break;

    case AMOUNT:
      if (as_amount().commodity() != val.as_amount().commodity()) {
	in_place_cast(BALANCE);
	return *this += val;
      } else {
	as_amount_lval() += val.as_amount();
	return *this;
      }
      break;

    case BALANCE:
      in_place_cast(BALANCE);
      as_balance_lval() += val.as_balance();
      return *this;

    case BALANCE_PAIR:
      in_place_cast(BALANCE_PAIR);
      as_balance_pair_lval() += val.as_balance_pair();
      return *this;
    default:
      break;
    }
    break;

  case BALANCE:
    switch (val.type()) {
    case INTEGER:
      as_balance_lval() += val.to_amount();
      return *this;
    case AMOUNT:
      as_balance_lval() += val.as_amount();
      return *this;
    case BALANCE:
      as_balance_lval() += val.as_balance();
      return *this;
    case BALANCE_PAIR:
      in_place_cast(BALANCE_PAIR);
      as_balance_pair_lval() += val.as_balance_pair();
      return *this;
    default:
      break;
    }
    break;

  case BALANCE_PAIR:
    switch (val.type()) {
    case INTEGER:
      as_balance_pair_lval() += val.to_amount();
      return *this;
    case AMOUNT:
      as_balance_pair_lval() += val.as_amount();
      return *this;
    case BALANCE:
      as_balance_pair_lval() += val.as_balance();
      return *this;
    case BALANCE_PAIR:
      as_balance_pair_lval() += val.as_balance_pair();
      return *this;
    default:
      break;
    }
    break;

  default:
    break;
  }

  throw_(value_error, "Cannot add " << label() << " to " << val.label());

  return *this;
}

value_t& value_t::operator-=(const value_t& val)
{
  if (is_sequence()) {
    sequence_t& seq(as_sequence_lval());

    if (val.is_sequence()) {
      for (sequence_t::const_iterator i = val.as_sequence().begin();
	   i != val.as_sequence().end();
	   i++) {
	sequence_t::iterator j = std::find(seq.begin(), seq.end(), *i);
	if (j != seq.end())
	  seq.erase(j);
      }
    } else {
      sequence_t::iterator i = std::find(seq.begin(), seq.end(), val);
      if (i != seq.end())
	seq.erase(i);
    }
    return *this;
  }

  if (val.is_xml_node()) // recurse
    return *this -= val.as_xml_node()->to_value();

  switch (type()) {
  case DATETIME:
    switch (val.type()) {
    case INTEGER:
      as_datetime_lval() -= date_duration(val.as_long());
      return *this;
    case AMOUNT:
      as_datetime_lval() -= date_duration(val.as_amount().to_long());
      return *this;
    default:
      break;
    }
    break;

  case INTEGER:
    switch (val.type()) {
    case INTEGER:
      as_long_lval() -= val.as_long();
      return *this;
    case AMOUNT:
      in_place_cast(AMOUNT);
      as_amount_lval() -= val.as_amount();
      in_place_simplify();
      return *this;
    case BALANCE:
      in_place_cast(BALANCE);
      as_balance_lval() -= val.as_balance();
      in_place_simplify();
      return *this;
    case BALANCE_PAIR:
      in_place_cast(BALANCE_PAIR);
      as_balance_pair_lval() -= val.as_balance_pair();
      in_place_simplify();
      return *this;
    default:
      break;
    }
    break;

  case AMOUNT:
    switch (val.type()) {
    case INTEGER:
      if (as_amount().has_commodity()) {
	in_place_cast(BALANCE);
	*this -= val;
	in_place_simplify();
	return *this;
      } else {
	as_amount_lval() -= val.as_long();
	in_place_simplify();
	return *this;
      }
      break;

    case AMOUNT:
      if (as_amount().commodity() != val.as_amount().commodity()) {
	in_place_cast(BALANCE);
	*this -= val;
	in_place_simplify();
	return *this;
      } else {
	as_amount_lval() -= val.as_amount();
	in_place_simplify();
	return *this;
      }
      break;

    case BALANCE:
      in_place_cast(BALANCE);
      as_balance_lval() -= val.as_balance();
      in_place_simplify();
      return *this;

    case BALANCE_PAIR:
      in_place_cast(BALANCE_PAIR);
      as_balance_pair_lval() -= val.as_balance_pair();
      in_place_simplify();
      return *this;
    default:
      break;
    }
    break;

  case BALANCE:
    switch (val.type()) {
    case INTEGER:
      as_balance_lval() -= val.to_amount();
      in_place_simplify();
      return *this;
    case AMOUNT:
      as_balance_lval() -= val.as_amount();
      in_place_simplify();
      return *this;
    case BALANCE:
      as_balance_lval() -= val.as_balance();
      in_place_simplify();
      return *this;
    case BALANCE_PAIR:
      in_place_cast(BALANCE_PAIR);
      as_balance_pair_lval() -= val.as_balance_pair();
      in_place_simplify();
      return *this;
    default:
      break;
    }
    break;

  case BALANCE_PAIR:
    switch (val.type()) {
    case INTEGER:
      as_balance_pair_lval() -= val.to_amount();
      in_place_simplify();
      return *this;
    case AMOUNT:
      as_balance_pair_lval() -= val.as_amount();
      in_place_simplify();
      return *this;
    case BALANCE:
      as_balance_pair_lval() -= val.as_balance();
      in_place_simplify();
      return *this;
    case BALANCE_PAIR:
      as_balance_pair_lval() -= val.as_balance_pair();
      in_place_simplify();
      return *this;
    default:
      break;
    }
    break;

  default:
    break;
  }

  throw_(value_error, "Cannot subtract " << label() << " from " << val.label());

  return *this;
}

value_t& value_t::operator*=(const value_t& val)
{
  if (is_string()) {
    string temp;
    long count = val.to_long();
    for (long i = 0; i < count; i++)
      temp += as_string();
    set_string(temp);
    return *this;
  }
  else if (is_sequence()) {
    value_t temp;
    long count = val.to_long();
    for (long i = 0; i < count; i++)
      temp += as_sequence();
    return *this = temp;
  }

  if (val.is_xml_node()) // recurse
    return *this *= val.as_xml_node()->to_value();

  switch (type()) {
  case INTEGER:
    switch (val.type()) {
    case INTEGER:
      as_long_lval() *= val.as_long();
      return *this;
    case AMOUNT:
      set_amount(val.as_amount() * as_long());
      return *this;
    default:
      break;
    }
    break;

  case AMOUNT:
    switch (val.type()) {
    case INTEGER:
      as_amount_lval() *= val.as_long();
      return *this;
    case AMOUNT:
      if (as_amount().commodity() == val.as_amount().commodity() ||
	  ! val.as_amount().has_commodity()) {
	as_amount_lval() *= val.as_amount();
	return *this;
      }
      break;
    default:
      break;
    }
    break;

  case BALANCE:
    switch (val.type()) {
    case INTEGER:
      as_balance_lval() *= val.as_long();
      return *this;
    case AMOUNT:
      if (! val.as_amount().has_commodity()) {
	as_balance_lval() *= val.as_amount();
	return *this;
      }
      break;
    default:
      break;
    }
    break;

  case BALANCE_PAIR:
    switch (val.type()) {
    case INTEGER:
      as_balance_pair_lval() *= val.as_long();
      return *this;
    case AMOUNT:
      if (! val.as_amount().has_commodity()) {
	as_balance_pair_lval() *= val.as_amount();
	return *this;
      }
      break;
    default:
      break;
    }
    break;

  default:
    break;
  }

  throw_(value_error, "Cannot multiply " << label() << " with " << val.label());

  return *this;
}

value_t& value_t::operator/=(const value_t& val)
{
  if (val.is_xml_node()) // recurse
    return *this /= val.as_xml_node()->to_value();

  switch (type()) {
  case INTEGER:
    switch (val.type()) {
    case INTEGER:
      as_long_lval() /= val.as_long();
      return *this;
    case AMOUNT:
      set_amount(val.as_amount() / as_long());
      return *this;
    default:
      break;
    }
    break;

  case AMOUNT:
    switch (val.type()) {
    case INTEGER:
      as_amount_lval() /= val.as_long();
      return *this;

    case AMOUNT:
      if (as_amount().commodity() == val.as_amount().commodity() ||
	  ! val.as_amount().has_commodity()) {
	as_amount_lval() /= val.as_amount();
	return *this;
      }
      break;
    default:
      break;
    }
    break;

  case BALANCE:
    switch (val.type()) {
    case INTEGER:
      as_balance_lval() /= val.as_long();
      return *this;
    case AMOUNT:
      if (! val.as_amount().has_commodity()) {
	as_balance_lval() /= val.as_amount();
	return *this;
      }
      break;
    default:
      break;
    }
    break;

  case BALANCE_PAIR:
    switch (val.type()) {
    case INTEGER:
      as_balance_pair_lval() /= val.as_long();
      return *this;
    case AMOUNT:
      if (! val.as_amount().has_commodity()) {
	as_balance_pair_lval() /= val.as_amount();
	return *this;
      }
      break;
    default:
      break;
    }
    break;

  default:
    break;
  }

  throw_(value_error, "Cannot divide " << label() << " by " << val.label());

  return *this;
}


bool value_t::operator==(const value_t& val) const
{
  if (is_xml_node() && val.is_xml_node())
    return as_xml_node() == val.as_xml_node();
  else if (is_xml_node())
    return as_xml_node()->to_value() == val;
  else if (val.is_xml_node())
    return *this == val.as_xml_node()->to_value();

  switch (type()) {
  case BOOLEAN:
    if (val.is_boolean())
      return as_boolean() == val.as_boolean();
    break;

  case DATETIME:
    if (val.is_datetime())
      return as_datetime() == val.as_datetime();
    break;

  case INTEGER:
    switch (val.type()) {
    case INTEGER:
      return as_long() == val.as_long();
    case AMOUNT:
      return val.as_amount() == to_amount();
    case BALANCE:
      return val.as_balance() == to_amount();
    case BALANCE_PAIR:
      return val.as_balance_pair() == to_amount();
    default:
      break;
    }
    break;

  case AMOUNT:
    switch (val.type()) {
    case INTEGER:
      return as_amount() == val.as_long();
    case AMOUNT:
      return as_amount() == val.as_amount();
    case BALANCE:
      return val.as_balance() == as_amount();
    case BALANCE_PAIR:
      return val.as_balance_pair() == as_amount();
    default:
      break;
    }
    break;

  case BALANCE:
    switch (val.type()) {
    case INTEGER:
      return as_balance() == val.to_amount();
    case AMOUNT:
      return as_balance() == val.as_amount();
    case BALANCE:
      return as_balance() == val.as_balance();
    case BALANCE_PAIR:
      return val.as_balance_pair() == as_balance();
    default:
      break;
    }
    break;

  case BALANCE_PAIR:
    switch (val.type()) {
    case INTEGER:
      return as_balance_pair() == val.to_amount();
    case AMOUNT:
      return as_balance_pair() == val.as_amount();
    case BALANCE:
      return as_balance_pair() == val.as_balance();
    case BALANCE_PAIR:
      return as_balance_pair() == val.as_balance_pair();
    default:
      break;
    }
    break;

  case STRING:
    if (val.is_string())
      return as_string() == val.as_string();
    break;

  case SEQUENCE:
    if (val.is_sequence())
      return as_sequence() == val.as_sequence();
    break;

  default:
    break;
  }

  throw_(value_error, "Cannot compare " << label() << " to " << val.label());

  return *this;
}

bool value_t::operator<(const value_t& val) const
{
  if (is_xml_node() && val.is_xml_node())
    return as_xml_node() < val.as_xml_node();
  else if (is_xml_node())
    return as_xml_node()->to_value() < val;
  else if (val.is_xml_node())
    return *this < val.as_xml_node()->to_value();

  switch (type()) {
  case DATETIME:
    if (val.is_datetime())
      return as_datetime() < val.as_datetime();
    break;

  case INTEGER:
    switch (val.type()) {
    case INTEGER:
      return as_long() < val.as_long();
    case AMOUNT:
      return val.as_amount() < as_long();
    default:
      break;
    }
    break;

  case AMOUNT:
    switch (val.type()) {
    case INTEGER:
      return as_amount() < val.as_long();
    case AMOUNT:
      return as_amount() < val.as_amount();
    default:
      break;
    }
    break;

  case STRING:
    if (val.is_string())
      return as_string() < val.as_string();
    break;

  default:
    break;
  }

  throw_(value_error, "Cannot compare " << label() << " to " << val.label());

  return *this;
}

#if 0
bool value_t::operator>(const value_t& val) const
{
  if (is_xml_node() && val.is_xml_node())
    return as_xml_node() > val.as_xml_node();
  else if (is_xml_node())
    return as_xml_node()->to_value() > val;
  else if (val.is_xml_node())
    return *this > val.as_xml_node()->to_value();

  switch (type()) {
  case DATETIME:
    if (val.is_datetime())
      return as_datetime() > val.as_datetime();
    break;

  case INTEGER:
    switch (val.type()) {
    case INTEGER:
      return as_long() > val.as_long();
    case AMOUNT:
      return val.as_amount() > as_long();
    default:
      break;
    }
    break;

  case AMOUNT:
    switch (val.type()) {
    case INTEGER:
      return as_amount() > val.as_long();
    case AMOUNT:
      return as_amount() > val.as_amount();
    default:
      break;
    }
    break;

  case STRING:
    if (val.is_string())
      return as_string() > val.as_string();
    break;

  default:
    break;
  }

  throw_(value_error,
	 "Cannot compare " << label() << " to " << val.label());

  return *this;
}
#endif

void value_t::in_place_cast(type_t cast_type)
{
  if (type() == cast_type)
    return;

  if (cast_type == BOOLEAN) {
    set_boolean(bool(*this));
    return;
  }
  else if (cast_type == SEQUENCE) {
    sequence_t temp;
    if (! is_null())
      temp.push_back(*this);
    set_sequence(temp);
    return;
  }

  // This must came after the if's above, otherwise it would be
  // impossible to turn an XML node into a sequence containing that
  // same XML node.
  if (is_xml_node()) {
    *this = as_xml_node()->to_value().cast(cast_type);
    return;
  }

  switch (type()) {
  case BOOLEAN:
    switch (cast_type) {
    case STRING:
      set_string(as_boolean() ? "true" : "false");
      return;
    default:
      break;
    }
    break;

  case INTEGER:
    switch (cast_type) {
    case AMOUNT:
      set_amount(as_long());
      return;
    case BALANCE:
      set_balance(to_amount());
      return;
    case BALANCE_PAIR:
      set_balance_pair(to_amount());
      return;
    case STRING:
      set_string(lexical_cast<string>(as_long()));
      return;
    default:
      break;
    }
    break;

  case AMOUNT:
    switch (cast_type) {
    case INTEGER:
      set_long(as_amount().to_long());
      return;
    case BALANCE:
      set_balance(as_amount());
      return;
    case BALANCE_PAIR:
      set_balance_pair(as_amount());
      return;
    case STRING:
      set_string(as_amount().to_string());
      return;
    default:
      break;
    }
    break;

  case BALANCE:
    switch (cast_type) {
    case AMOUNT: {
      const balance_t& temp(as_balance());
      if (temp.amounts.size() == 1) {
	set_amount((*temp.amounts.begin()).second);
	return;
      }
      else if (temp.amounts.size() == 0) {
	set_amount(0L);
	return;
      }
      else {
	throw_(value_error, "Cannot convert " << label() <<
	       " with multiple commodities to " << label(cast_type));
      }
      break;
    }
    case BALANCE_PAIR:
      set_balance_pair(as_balance());
      return;
    default:
      break;
    }
    break;

  case BALANCE_PAIR:
    switch (cast_type) {
    case AMOUNT: {
      const balance_t& temp(as_balance_pair().quantity);
      if (temp.amounts.size() == 1) {
	set_amount((*temp.amounts.begin()).second);
	return;
      }
      else if (temp.amounts.size() == 0) {
	set_amount(0L);
	return;
      }
      else {
	throw_(value_error, "Cannot convert " << label() <<
	       " with multiple commodities to " << label(cast_type));
      }
      break;
    }
    case BALANCE:
      set_balance(as_balance_pair().quantity);
      return;
    default:
      break;
    }
    break;

  case STRING:
    switch (cast_type) {
    case INTEGER: {
      if (all(as_string(), is_digit())) {
	set_long(lexical_cast<long>(as_string()));
	return;
      } else {
	throw_(value_error,
	       "Cannot convert string '" << *this << "' to an integer");
      }
      break;
    }
    case AMOUNT:
      set_amount(amount_t(as_string()));
      return;
    default:
      break;
    }
    break;

  default:
    break;
  }

  throw_(value_error,
	 "Cannot convert " << label() << " to " << label(cast_type));
}

void value_t::in_place_negate()
{
  switch (type()) {
  case BOOLEAN:
    set_boolean(! as_boolean());
    return;
  case INTEGER:
    set_long(- as_long());
    return;
  case AMOUNT:
    as_amount_lval().in_place_negate();
    return;
  case BALANCE:
    as_balance_lval().in_place_negate();
    return;
  case BALANCE_PAIR:
    as_balance_pair_lval().in_place_negate();
    return;
  case XML_NODE:
    *this = as_xml_node()->to_value();
    in_place_negate();
    return;
  default:
    break;
  }

  throw_(value_error, "Cannot negate " << label());
}

bool value_t::is_realzero() const
{
  switch (type()) {
  case BOOLEAN:
    return ! as_boolean();
  case INTEGER:
    return as_long() == 0;
  case DATETIME:
    return ! is_valid_moment(as_datetime());
  case AMOUNT:
    return as_amount().is_realzero();
  case BALANCE:
    return as_balance().is_realzero();
  case BALANCE_PAIR:
    return as_balance_pair().is_realzero();
  case STRING:
    return as_string().empty();
  case SEQUENCE:
    return as_sequence().empty();

  case XML_NODE:
    return as_xml_node() == NULL;
  case POINTER:
    return as_any_pointer().empty();

  default:
    assert(false);
    break;
  }
  assert(false);
  return true;
}

value_t value_t::value(const optional<moment_t>& moment) const
{
  switch (type()) {
  case INTEGER:
    return *this;

  case AMOUNT: {
    if (optional<amount_t> val = as_amount().value(moment))
      return *val;
    return false;
  }
  case BALANCE: {
    if (optional<balance_t> bal = as_balance().value(moment))
      return *bal;
    return false;
  }
  case BALANCE_PAIR: {
    if (optional<balance_t> bal_pair =
	as_balance_pair().quantity.value(moment))
      return *bal_pair;
    return false;
  }
  case XML_NODE:
    return as_xml_node()->to_value().value(moment);

  default:
    break;
  }

  throw_(value_error, "Cannot find the value of " << label());
  return value_t();
}

void value_t::in_place_reduce()
{
  switch (type()) {
  case INTEGER:
    return;
  case AMOUNT:
    as_amount_lval().in_place_reduce();
    return;
  case BALANCE:
    as_balance_lval().in_place_reduce();
    return;
  case BALANCE_PAIR:
    as_balance_pair_lval().in_place_reduce();
    return;
  case XML_NODE:
    *this = as_xml_node()->to_value();
    in_place_reduce();		// recurse
    return;
  default:
    break;
  }

  throw_(value_error, "Cannot reduce " << label());
}

value_t value_t::round() const
{
  switch (type()) {
  case INTEGER:
    return *this;
  case AMOUNT:
    return as_amount().round();
  case XML_NODE:
    return as_xml_node()->to_value().round();
  default:
    break;
  }

  throw_(value_error, "Cannot round " << label());
  return value_t();
}

value_t value_t::unround() const
{
  switch (type()) {
  case INTEGER:
    return *this;
  case AMOUNT:
    return as_amount().unround();
  case XML_NODE:
    return as_xml_node()->to_value().unround();
  default:
    break;
  }

  throw_(value_error, "Cannot unround " << label());
  return value_t();
}

value_t value_t::annotated_price() const
{
  switch (type()) {
  case AMOUNT: {
    optional<amount_t> temp = as_amount().annotation_details().price;
    if (! temp)
      return false;
    return *temp;
  }

  case XML_NODE:
    return as_xml_node()->to_value().annotated_price();

  default:
    break;
  }

  throw_(value_error, "Cannot find the annotated price of " << label());
  return value_t();
}

value_t value_t::annotated_date() const
{
  switch (type()) {
  case DATETIME:
    return *this;

  case AMOUNT: {
    optional<moment_t> temp = as_amount().annotation_details().date;
    if (! temp)
      return false;
    return *temp;
  }

  case XML_NODE:
    return as_xml_node()->to_value().annotated_date();

  default:
    break;
  }

  throw_(value_error, "Cannot find the annotated date of " << label());
  return value_t();
}

value_t value_t::annotated_tag() const
{
  switch (type()) {
  case DATETIME:
    return *this;

  case AMOUNT: {
    optional<string> temp = as_amount().annotation_details().tag;
    if (! temp)
      return false;
    return value_t(*temp, true);
  }

  case XML_NODE:
    return as_xml_node()->to_value().annotated_tag();

  default:
    break;
  }

  throw_(value_error, "Cannot find the annotated tag of " << label());
  return value_t();
}

value_t value_t::strip_annotations(const bool keep_price,
				   const bool keep_date,
				   const bool keep_tag) const
{
  switch (type()) {
  case VOID:
  case BOOLEAN:
  case INTEGER:
  case DATETIME:
  case STRING:
  case XML_NODE:
  case POINTER:
    return *this;

  case SEQUENCE: {
    sequence_t temp;
    foreach (const value_t& value, as_sequence())
      temp.push_back(value.strip_annotations(keep_price, keep_date, keep_tag));
    return temp;
  }

  case AMOUNT:
    return as_amount().strip_annotations(keep_price, keep_date, keep_tag);
  case BALANCE:
    return as_balance().strip_annotations(keep_price, keep_date, keep_tag);
  case BALANCE_PAIR:
    return as_balance_pair().quantity.strip_annotations(keep_price,
							keep_date, keep_tag);

  default:
    assert(false);
    break;
  }
  assert(false);
  return value_t();
}

value_t value_t::cost() const
{
  switch (type()) {
  case INTEGER:
  case AMOUNT:
  case BALANCE:
    return *this;

  case BALANCE_PAIR:
    assert(as_balance_pair().cost);
    if (as_balance_pair().cost)
      return *(as_balance_pair().cost);
    else
      return as_balance_pair().quantity;

  case XML_NODE:
    return as_xml_node()->to_value().cost();

  default:
    break;
  }

  throw_(value_error, "Cannot find the cost of " << label());
  return value_t();
}

value_t& value_t::add(const amount_t& amount, const optional<amount_t>& tcost)
{
  switch (type()) {
  case INTEGER:
  case AMOUNT:
    if (tcost) {
      in_place_cast(BALANCE_PAIR);
      return add(amount, tcost);
    }
    else if ((is_amount() &&
	      as_amount().commodity() != amount.commodity()) ||
	     (! is_amount() && amount.commodity())) {
      in_place_cast(BALANCE);
      return add(amount, tcost);
    }
    else if (! is_amount()) {
      in_place_cast(AMOUNT);
    }
    *this += amount;
    break;

  case BALANCE:
    if (tcost) {
      in_place_cast(BALANCE_PAIR);
      return add(amount, tcost);
    }
    *this += amount;
    break;

  case BALANCE_PAIR:
    as_balance_pair_lval().add(amount, tcost);
    break;

  default:
    break;
  }

  throw_(value_error, "Cannot add an amount to " << label());
  return *this;
}

void value_t::print(std::ostream& out, const int first_width,
		    const int latter_width) const
{
  switch (type()) {
  case VOID:
    out << "NULL";
    break;

  case BOOLEAN:
  case DATETIME:
  case INTEGER:
  case AMOUNT:
  case STRING:
  case POINTER:
    // jww (2007-05-14): I need a version of this print just for XPath
    // expression, since amounts and strings need to be output with
    // special syntax.
    out << *this;
    break;

  case XML_NODE:
    as_xml_node()->print(out);
    break;

  case SEQUENCE: {
    out << '(';
    bool first = true;
    foreach (const value_t& value, as_sequence()) {
      if (first)
	first = false;
      else
	out << ", ";

      value.print(out, first_width, latter_width);
    }
    out << ')';
    break;
  }

  case BALANCE:
    as_balance().print(out, first_width, latter_width);
    break;
  case BALANCE_PAIR:
    as_balance_pair().print(out, first_width, latter_width);
    break;
  default:
    assert(false);
    break;
  }
}

std::ostream& operator<<(std::ostream& out, const value_t& val)
{
  switch (val.type()) {
  case value_t::VOID:
    out << "VOID";
    break;
  case value_t::BOOLEAN:
    out << (val.as_boolean() ? "true" : "false");
    break;
  case value_t::INTEGER:
    out << val.as_long();
    break;
  case value_t::DATETIME:
    out << val.as_datetime();
    break;
  case value_t::AMOUNT:
    out << val.as_amount();
    break;
  case value_t::BALANCE:
    out << val.as_balance();
    break;
  case value_t::BALANCE_PAIR:
    out << val.as_balance_pair();
    break;
  case value_t::STRING:
    out << val.as_string();
    break;
  case value_t::XML_NODE:
    if (val.as_xml_node()->has_flags(XML_NODE_IS_PARENT))
      out << '<' << val.as_xml_node()->name() << '>';
    else
      out << val.as_xml_node()->to_value();
    break;

  case value_t::POINTER:
    throw_(value_error, "Cannot output a pointer value");

  case value_t::SEQUENCE: {
    out << '(';
    bool first = true;
    for (value_t::sequence_t::const_iterator i = val.as_sequence().begin();
	 i != val.as_sequence().end();
	 i++) {
      if (first)
	first = false;
      else
	out << ", ";
      out << *i;
    }
    out << ')';
    break;
  }

  default:
    assert(false);
    break;
  }
  return out;
}

#if 0
value_context::value_context(const value_t& _bal,
			     const string& _desc) throw()
  : error_context(_desc), bal(new value_t(_bal)) {}

value_context::~value_context() throw()
{
  checked_delete(bal);
}

void value_context::describe(std::ostream& out) const throw()
{
  if (! desc.empty())
    out << desc << std::endl;

  balance_t * ptr = NULL;

  out << std::right;
  out.width(20);

  switch (bal->type()) {
  case value_t::BOOLEAN:
    out << (*((bool *) bal->data) ? "true" : "false");
    break;
  case value_t::INTEGER:
    out << *((long *) bal->data);
    break;
  case value_t::DATETIME:
    out << *((moment_t *) bal->data);
    break;
  case value_t::AMOUNT:
    out << *((amount_t *) bal->data);
    break;
  case value_t::BALANCE:
    ptr = (balance_t *) bal->data;
    // fall through...

  case value_t::BALANCE_PAIR:
    if (! ptr)
      ptr = &((balance_pair_t *) bal->data)->quantity;

    ptr->print(out, 20);
    break;
  default:
    assert(false);
    break;
  }
  out << std::endl;
}
#endif

} // namespace ledger
