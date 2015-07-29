#include <iostream>
#include <valhalla/midgard/logging.h>

#include "odin/narrativebuilder.h"
#include "odin/maneuver.h"

#include <boost/format.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace {
// Text instruction initial capacity
constexpr auto kTextInstructionInitialCapacity = 128;
}

namespace valhalla {
namespace odin {

void NarrativeBuilder::Build(const DirectionsOptions& directions_options,
                             std::list<Maneuver>& maneuvers) {
  Maneuver* prev_maneuver = nullptr;
  for (auto& maneuver : maneuvers) {
    switch (maneuver.type()) {
      case TripDirections_Maneuver_Type_kStartRight:
      case TripDirections_Maneuver_Type_kStart:
      case TripDirections_Maneuver_Type_kStartLeft: {
        // Set instruction
        maneuver.set_instruction(std::move(FormStartInstruction(maneuver)));

        // Set verbal pre transition instruction
        maneuver.set_verbal_pre_transition_instruction(
            std::move(FormVerbalStartInstruction(maneuver)));

        // Set verbal post transition instruction
        maneuver.set_verbal_post_transition_instruction(
            std::move(
                FormVerbalPostTransitionInstruction(
                    maneuver, directions_options.units(),
                    maneuver.HasBeginStreetNames())));
        break;
      }
      case TripDirections_Maneuver_Type_kDestinationRight:
      case TripDirections_Maneuver_Type_kDestination:
      case TripDirections_Maneuver_Type_kDestinationLeft: {
        // Set instruction
        maneuver.set_instruction(std::move(FormDestinationInstruction(maneuver)));

        // Set verbal transition alert instruction
        maneuver.set_verbal_transition_alert_instruction(
            std::move(FormVerbalAlertDestinationInstruction(maneuver)));

        // Set verbal pre transition instruction
        maneuver.set_verbal_pre_transition_instruction(
            std::move(FormVerbalDestinationInstruction(maneuver)));
        break;
      }
      case TripDirections_Maneuver_Type_kBecomes: {
        // Set instruction
        maneuver.set_instruction(
            std::move(FormBecomesInstruction(maneuver, prev_maneuver)));

        // Set verbal pre transition instruction
        maneuver.set_verbal_pre_transition_instruction(
            std::move(FormVerbalBecomesInstruction(maneuver, prev_maneuver)));

        // Set verbal post transition instruction
        maneuver.set_verbal_post_transition_instruction(
            std::move(
                FormVerbalPostTransitionInstruction(
                    maneuver, directions_options.units(),
                    maneuver.HasBeginStreetNames())));
        break;
      }
      case TripDirections_Maneuver_Type_kContinue: {
        // Set instruction
        maneuver.set_instruction(std::move(FormContinueInstruction(maneuver)));

        // Set verbal transition alert instruction
        maneuver.set_verbal_transition_alert_instruction(
            std::move(FormVerbalAlertContinueInstruction(maneuver)));

        // Set verbal pre transition instruction
        maneuver.set_verbal_pre_transition_instruction(
            std::move(FormVerbalContinueInstruction(maneuver)));

        // Set verbal post transition instruction
        maneuver.set_verbal_post_transition_instruction(
            std::move(
                FormVerbalPostTransitionInstruction(
                    maneuver, directions_options.units())));
        break;
      }
      case TripDirections_Maneuver_Type_kSlightRight:
      case TripDirections_Maneuver_Type_kSlightLeft: {
        if (maneuver.HasSimilarNames(prev_maneuver, true)) {
          // Set stay on instruction
          maneuver.set_instruction(
              std::move(FormBearToStayOnInstruction(maneuver)));

          // Set verbal transition alert instruction
          maneuver.set_verbal_transition_alert_instruction(
              std::move(FormVerbalAlertBearToStayOnInstruction(maneuver)));

          // Set verbal pre transition instruction
          maneuver.set_verbal_pre_transition_instruction(
              std::move(FormVerbalBearToStayOnInstruction(maneuver)));

          // Set verbal post transition instruction
          maneuver.set_verbal_post_transition_instruction(
              std::move(
                  FormVerbalPostTransitionInstruction(
                      maneuver, directions_options.units())));
        } else {
          FormBearInstruction(maneuver);
          // Set instruction
          maneuver.set_instruction(std::move(FormBearInstruction(maneuver)));

          // Set verbal transition alert instruction
          maneuver.set_verbal_transition_alert_instruction(
              std::move(FormVerbalAlertBearInstruction(maneuver)));

          // Set verbal pre transition instruction
          maneuver.set_verbal_pre_transition_instruction(
              std::move(FormVerbalBearInstruction(maneuver)));

          // Set verbal post transition instruction
          maneuver.set_verbal_post_transition_instruction(
              std::move(
                  FormVerbalPostTransitionInstruction(
                      maneuver, directions_options.units(),
                      maneuver.HasBeginStreetNames())));
        }
        break;
      }
      case TripDirections_Maneuver_Type_kRight:
      case TripDirections_Maneuver_Type_kSharpRight:
      case TripDirections_Maneuver_Type_kSharpLeft:
      case TripDirections_Maneuver_Type_kLeft: {
        if (maneuver.HasSimilarNames(prev_maneuver, true)) {
          // Set stay on instruction
          maneuver.set_instruction(
              std::move(FormTurnToStayOnInstruction(maneuver)));

          // Set verbal transition alert instruction
          maneuver.set_verbal_transition_alert_instruction(
              std::move(FormVerbalAlertTurnToStayOnInstruction(maneuver)));

          // Set verbal pre transition instruction
          maneuver.set_verbal_pre_transition_instruction(
              std::move(FormVerbalTurnToStayOnInstruction(maneuver)));

          // Set verbal post transition instruction
          maneuver.set_verbal_post_transition_instruction(
              std::move(
                  FormVerbalPostTransitionInstruction(
                      maneuver, directions_options.units())));
        } else {
          // Set instruction
          maneuver.set_instruction(std::move(FormTurnInstruction(maneuver)));

          // Set verbal transition alert instruction
          maneuver.set_verbal_transition_alert_instruction(
              std::move(FormVerbalAlertTurnInstruction(maneuver)));

          // Set verbal pre transition instruction
          maneuver.set_verbal_pre_transition_instruction(
              std::move(FormVerbalTurnInstruction(maneuver)));

          // Set verbal post transition instruction
          maneuver.set_verbal_post_transition_instruction(
              std::move(
                  FormVerbalPostTransitionInstruction(
                      maneuver, directions_options.units(),
                      maneuver.HasBeginStreetNames())));
        }
        break;
      }
      case TripDirections_Maneuver_Type_kUturnRight:
      case TripDirections_Maneuver_Type_kUturnLeft: {
        // Set instruction
        maneuver.set_instruction(
            std::move(FormUturnInstruction(maneuver)));

        // Set verbal transition alert instruction
        maneuver.set_verbal_transition_alert_instruction(
            std::move(FormVerbalAlertUturnInstruction(maneuver)));

        // Set verbal pre transition instruction
        maneuver.set_verbal_pre_transition_instruction(
            std::move(FormVerbalUturnInstruction(maneuver)));

        // Set verbal post transition instruction
        maneuver.set_verbal_post_transition_instruction(
            std::move(
                FormVerbalPostTransitionInstruction(
                    maneuver, directions_options.units())));
        break;
      }
      case TripDirections_Maneuver_Type_kRampStraight: {
        // Set instruction
        maneuver.set_instruction(
            std::move(FormRampStraightInstruction(maneuver)));

        // Set verbal transition alert instruction
        maneuver.set_verbal_transition_alert_instruction(
            std::move(FormVerbalAlertRampStraightInstruction(maneuver)));

        // Set verbal pre transition instruction
        maneuver.set_verbal_pre_transition_instruction(
            std::move(FormVerbalRampStraightInstruction(maneuver)));

        // Only set verbal post if > min ramp length
        if (maneuver.length() > kVerbalPostMinimumRampLength) {
          // Set verbal post transition instruction
          maneuver.set_verbal_post_transition_instruction(
              std::move(
                  FormVerbalPostTransitionInstruction(
                      maneuver, directions_options.units())));
        }
        break;
      }
      case TripDirections_Maneuver_Type_kRampRight:
      case TripDirections_Maneuver_Type_kRampLeft: {
        // Set instruction
        maneuver.set_instruction(std::move(FormRampInstruction(maneuver)));

        // Set verbal transition alert instruction
        maneuver.set_verbal_transition_alert_instruction(
            std::move(FormVerbalAlertRampInstruction(maneuver)));

        // Set verbal pre transition instruction
        maneuver.set_verbal_pre_transition_instruction(
            std::move(FormVerbalRampInstruction(maneuver)));

        // Only set verbal post if > min ramp length
        if (maneuver.length() > kVerbalPostMinimumRampLength) {
          // Set verbal post transition instruction
          maneuver.set_verbal_post_transition_instruction(
              std::move(
                  FormVerbalPostTransitionInstruction(
                      maneuver, directions_options.units())));
        }
        break;
      }
      case TripDirections_Maneuver_Type_kExitRight:
      case TripDirections_Maneuver_Type_kExitLeft: {
        // Set instruction
        maneuver.set_instruction(std::move(FormExitInstruction(maneuver)));
        break;
      }
      case TripDirections_Maneuver_Type_kStayStraight: {
        if (maneuver.HasSimilarNames(prev_maneuver)) {
          // Call stay on instruction
          FormStayStraightToStayOnInstruction(maneuver);
        } else {
          FormStayStraightInstruction(maneuver);
        }
        break;
      }
      case TripDirections_Maneuver_Type_kStayRight: {
        if (maneuver.HasSimilarNames(prev_maneuver)) {
          // Call stay on instruction
          FormStayRightToStayOnInstruction(maneuver);
        } else {
          FormStayRightInstruction(maneuver);
        }
        break;
      }
      case TripDirections_Maneuver_Type_kStayLeft: {
        if (maneuver.HasSimilarNames(prev_maneuver)) {
          // Call stay on instruction
          FormStayLeftToStayOnInstruction(maneuver);
        } else {
          FormStayLeftInstruction(maneuver);
        }
        break;
      }
      case TripDirections_Maneuver_Type_kMerge: {
        FormMergeInstruction(maneuver);
        break;
      }
      case TripDirections_Maneuver_Type_kRoundaboutEnter: {
        FormEnterRoundaboutInstruction(maneuver);
        break;
      }
      case TripDirections_Maneuver_Type_kRoundaboutExit: {
        FormExitRoundaboutInstruction(maneuver);
        break;
      }
      case TripDirections_Maneuver_Type_kFerryEnter: {
        FormEnterFerryInstruction(maneuver);
        break;
      }
      case TripDirections_Maneuver_Type_kFerryExit: {
        FormExitFerryInstruction(maneuver);
        break;
      }
      case TripDirections_Maneuver_Type_kTransitConnectionStart: {
        FormTransitConnectionStartInstruction(maneuver);
        break;
      }
      case TripDirections_Maneuver_Type_kTransitConnectionTransfer: {
        FormTransitConnectionTransferInstruction(maneuver);
        break;
      }
      case TripDirections_Maneuver_Type_kTransitConnectionDestination: {
        FormTransitConnectionDestinationInstruction(maneuver);
        break;
      }
      case TripDirections_Maneuver_Type_kTransit: {
        FormTransitInstruction(maneuver);
        break;
      }
      case TripDirections_Maneuver_Type_kTransitRemainOn: {
        FormTransitRemainOnInstruction(maneuver);
        break;
      }
      case TripDirections_Maneuver_Type_kTransitTransfer: {
        FormTransitTransferInstruction(maneuver);
        break;
      }
      case TripDirections_Maneuver_Type_kPostTransitConnectionDestination: {
        FormPostTransitConnectionDestinationInstruction(maneuver);
        break;
      }
      default: {
        FormContinueInstruction(maneuver);
        break;
      }
    }
    // Update previous maneuver
    prev_maneuver = &maneuver;
  }
}

NarrativeBuilder::NarrativeBuilder() {
}

// TODO - we will have to optimize when we actually use the language specific
// dictionary

std::string NarrativeBuilder::FormStartInstruction(Maneuver& maneuver) {
  // 0 "Go <FormCardinalDirection>."
  // 1 "Go <FormCardinalDirection> on <STREET_NAMES>."
  // 2 "Go <FormCardinalDirection> on <BEGIN_STREET_NAMES>. Continue on <STREET_NAMES>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);

  std::string cardinal_direction = FormCardinalDirection(
      maneuver.begin_cardinal_direction());
  std::string street_names;
  std::string begin_street_names;
  uint8_t phrase_id = 0;

  if (maneuver.HasStreetNames()) {
    phrase_id += 1;
    street_names = maneuver.street_names().ToString();
  }
  if (maneuver.HasBeginStreetNames()) {
    phrase_id += 1;
    begin_street_names = maneuver.begin_street_names().ToString();
  }

  switch (phrase_id) {
    // 1 "Go <FormCardinalDirection> on <STREET_NAMES>."
    case 1: {
      instruction = (boost::format("Go %1% on %2%.")
          % cardinal_direction % street_names).str();
      break;
    }
    // 2 "Go <FormCardinalDirection> on <BEGIN_STREET_NAMES>. Continue on <STREET_NAMES>."
    case 2: {
      instruction = (boost::format("Go %1% on %2%. Continue on %3%.")
          % cardinal_direction % begin_street_names % street_names).str();
      break;
    }
    // 0 "Go <FormCardinalDirection>."
    default: {
      instruction = (boost::format("Go %1%.") % cardinal_direction).str();
      break;
    }
  }
  // TODO - side of street

  return instruction;
}

std::string NarrativeBuilder::FormVerbalStartInstruction(
    Maneuver& maneuver, uint32_t element_max_count, std::string delim) {
  // 0 "Go <FormCardinalDirection>."
  // 1 "Go <FormCardinalDirection> on <STREET_NAMES|BEGIN_STREET_NAMES>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);

  std::string cardinal_direction = FormCardinalDirection(
      maneuver.begin_cardinal_direction());
  std::string street_names;
  uint8_t phrase_id = 0;

  if (maneuver.HasBeginStreetNames()) {
    phrase_id = 1;
    street_names = maneuver.begin_street_names().ToString(element_max_count,
                                                          delim);
  } else if (maneuver.HasStreetNames()) {
    phrase_id = 1;
    street_names = maneuver.street_names().ToString(element_max_count,
                                                    delim);
  }

  switch (phrase_id) {
    // 1 "Go <FormCardinalDirection> on <STREET_NAMES|BEGIN_STREET_NAMES>."
    case 1: {
      instruction = (boost::format("Go %1% on %2%.")
          % cardinal_direction % street_names).str();
      break;
    }
    // 0 "Go <FormCardinalDirection>."
    default: {
      instruction = (boost::format("Go %1%.") % cardinal_direction).str();
      break;
    }
  }
  // TODO - side of street

  return instruction;
}

std::string NarrativeBuilder::FormDestinationInstruction(Maneuver& maneuver) {
  // 0 "You have arrived at your destination."

  // TODO
  //  "You have arrived at <LOCATION_NAME|LOCATION_STREET_ADDRESS>."
  //  "Your destination is on the <SOS>."
  //  "<LOCATION_NAME|LOCATION_STREET_ADDRESS> is on the <SOS>."
  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction = "You have arrived at your destination.";

  return instruction;
}

std::string NarrativeBuilder::FormVerbalAlertDestinationInstruction(
    Maneuver& maneuver) {
  // 0 "You will arrive at your destination."

  // TODO
  //  "You will arrive at <LOCATION_NAME|LOCATION_STREET_ADDRESS>."
  //  "Your destination will be on the <SOS>."
  //  "<LOCATION_NAME|LOCATION_STREET_ADDRESS> will be on the <SOS>"

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction = "You will arrive at your destination.";

  return instruction;
}

std::string NarrativeBuilder::FormVerbalDestinationInstruction(
    Maneuver& maneuver) {
  // 0 "You have arrived at your destination."

  // TODO
  //  "You have arrived at <LOCATION_NAME|LOCATION_STREET_ADDRESS>."
  //  "Your destination is on the <SOS>."
  //  "<LOCATION_NAME|LOCATION_STREET_ADDRESS> is on the <SOS>."
  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction = "You have arrived at your destination.";

  return instruction;
}

std::string NarrativeBuilder::FormBecomesInstruction(Maneuver& maneuver,
                                              Maneuver* prev_maneuver) {
  // "<PREV_STREET_NAMES> becomes <STREET_NAMES>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);

  // If previous maneuver has names
  // and current maneuver has names
  // then form "becomes" narrative
  if (prev_maneuver && prev_maneuver->HasStreetNames()
      && maneuver.HasStreetNames()) {
    instruction += prev_maneuver->street_names().ToString();
    instruction += " becomes ";
    instruction += maneuver.street_names().ToString();
  }
  // Items are missing - fallback to just "Continue" narrative
  else {
    instruction += "Continue";

    if (maneuver.HasStreetNames()) {
      instruction += " on ";
      instruction += maneuver.street_names().ToString();
    }
  }

  instruction += ".";
  return instruction;
}

std::string NarrativeBuilder::FormVerbalBecomesInstruction(
    Maneuver& maneuver, Maneuver* prev_maneuver, uint32_t element_max_count,
    std::string delim) {
  // "<PREV_STREET_NAMES(2)> becomes <STREET_NAMES(2)>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);

  // If previous maneuver has names
  // and current maneuver has names
  // then form "becomes" narrative
  if (prev_maneuver && prev_maneuver->HasStreetNames()
      && maneuver.HasStreetNames()) {
    instruction += prev_maneuver->street_names().ToString(element_max_count,
                                                          delim);
    instruction += " becomes ";
    instruction += maneuver.street_names().ToString(element_max_count,
                                                    delim);
  }
  // Items are missing - fallback to just "Continue" narrative
  else {
    instruction += "Continue";

    if (maneuver.HasStreetNames()) {
      instruction += " on ";
      instruction += maneuver.street_names().ToString(element_max_count,
                                                      delim);
    }
  }

  instruction += ".";
  return instruction;
}

std::string NarrativeBuilder::FormContinueInstruction(Maneuver& maneuver) {
  // "Continue"
  // "Continue on <STREET_NAMES>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction += "Continue";

  if (maneuver.HasStreetNames()) {
    instruction += " on ";
    instruction += maneuver.street_names().ToString();
  }

  instruction += ".";
  return instruction;
}

std::string NarrativeBuilder::FormVerbalAlertContinueInstruction(
    Maneuver& maneuver, uint32_t element_max_count, std::string delim) {
  //  "Continue"
  //  "Continue on <STREET_NAMES(1)>."
  return FormVerbalContinueInstruction(maneuver, element_max_count, delim);
}

std::string NarrativeBuilder::FormVerbalContinueInstruction(
    Maneuver& maneuver, uint32_t element_max_count, std::string delim) {
  //  "Continue"
  //  "Continue on <STREET_NAMES(2)>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction += "Continue";

  if (maneuver.HasStreetNames()) {
    instruction += " on ";
    instruction += maneuver.street_names().ToString(element_max_count,
                                                    delim);
  }

  instruction += ".";
  return instruction;
}

std::string NarrativeBuilder::FormTurnInstruction(Maneuver& maneuver) {
  //  "Turn <FormTurnTypeInstruction>."
  //  "Turn <FormTurnTypeInstruction> onto <STREET_NAMES>."
  //  "Turn <FormTurnTypeInstruction> onto <BEGIN_STREET_NAMES>. Continue on <STREET_NAMES>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction += "Turn ";
  instruction += FormTurnTypeInstruction(maneuver.type());

  if (maneuver.HasBeginStreetNames()) {
    instruction += " onto ";
    instruction += maneuver.begin_street_names().ToString();
    instruction += ". Continue on ";
    instruction += maneuver.street_names().ToString();
  } else if (maneuver.HasStreetNames()) {
    instruction += " onto ";
    instruction += maneuver.street_names().ToString();
  }

  instruction += ".";
  return instruction;
}

std::string NarrativeBuilder::FormVerbalAlertTurnInstruction(
    Maneuver& maneuver, uint32_t element_max_count, std::string delim) {
    //    "Turn <FormTurnTypeInstruction>."
    //    "Turn <FormTurnTypeInstruction> onto <STREET_NAMES(1)|BEGIN_STREET_NAMES(1)>."

  return FormVerbalTurnInstruction(maneuver, element_max_count, delim);
}

std::string NarrativeBuilder::FormVerbalTurnInstruction(
    Maneuver& maneuver, uint32_t element_max_count, std::string delim) {
  //  "Turn <FormTurnTypeInstruction>."
  //  "Turn <FormTurnTypeInstruction> onto <STREET_NAMES(2)|BEGIN_STREET_NAMES(2)>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction += "Turn ";
  instruction += FormTurnTypeInstruction(maneuver.type());

  if (maneuver.HasBeginStreetNames()) {
    instruction += " onto ";
    instruction += maneuver.begin_street_names().ToString(element_max_count,
                                                          delim);
  } else if (maneuver.HasStreetNames()) {
    instruction += " onto ";
    instruction += maneuver.street_names().ToString(element_max_count,
                                                    delim);
  }

  instruction += ".";
  return instruction;
}

std::string NarrativeBuilder::FormTurnToStayOnInstruction(Maneuver& maneuver) {
  // "Turn <FormTurnTypeInstruction> to stay on <STREET_NAMES>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction += "Turn ";
  instruction += FormTurnTypeInstruction(maneuver.type());

  if (maneuver.HasStreetNames()) {
    instruction += " to stay on ";
    instruction += maneuver.street_names().ToString();
  }

  instruction += ".";
  return instruction;
}

std::string NarrativeBuilder::FormVerbalAlertTurnToStayOnInstruction(
    Maneuver& maneuver, uint32_t element_max_count, std::string delim) {
  // "Turn <FormTurnTypeInstruction> to stay on <STREET_NAMES(2)>."

  return FormVerbalTurnToStayOnInstruction(maneuver, element_max_count,
                                           delim);
}

std::string NarrativeBuilder::FormVerbalTurnToStayOnInstruction(
    Maneuver& maneuver, uint32_t element_max_count, std::string delim) {
  // "Turn <FormTurnTypeInstruction> to stay on <STREET_NAMES(2)>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction += "Turn ";
  instruction += FormTurnTypeInstruction(maneuver.type());

  if (maneuver.HasStreetNames()) {
    instruction += " to stay on ";
    instruction += maneuver.street_names().ToString(element_max_count,
                                                    delim);
  }

  instruction += ".";
  return instruction;
}

std::string NarrativeBuilder::FormBearInstruction(Maneuver& maneuver) {
  //  "Bear <FormTurnTypeInstruction>."
  //  "Bear <FormTurnTypeInstruction> onto <STREET_NAMES>."
  //  "Bear <FormTurnTypeInstruction> onto <BEGIN_STREET_NAMES>. Continue on <STREET_NAMES>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction += "Bear ";
  instruction += FormTurnTypeInstruction(maneuver.type());

  if (maneuver.HasBeginStreetNames()) {
    instruction += " onto ";
    instruction += maneuver.begin_street_names().ToString();
    instruction += ". Continue on ";
    instruction += maneuver.street_names().ToString();
  } else if (maneuver.HasStreetNames()) {
    instruction += " onto ";
    instruction += maneuver.street_names().ToString();
  }

  instruction += ".";
  return instruction;
}

std::string NarrativeBuilder::FormVerbalAlertBearInstruction(
    Maneuver& maneuver, uint32_t element_max_count, std::string delim) {
  //  "Bear <FormTurnTypeInstruction>."
  //  "Bear <FormTurnTypeInstruction> onto <STREET_NAMES(1)|BEGIN_STREET_NAMES(1)>."

  return FormVerbalBearInstruction(maneuver, element_max_count, delim);
}

std::string NarrativeBuilder::FormVerbalBearInstruction(
    Maneuver& maneuver, uint32_t element_max_count, std::string delim) {
  //  "Bear <FormTurnTypeInstruction>."
  //  "Bear <FormTurnTypeInstruction> onto <STREET_NAMES(2)|BEGIN_STREET_NAMES(2)>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction += "Bear ";
  instruction += FormTurnTypeInstruction(maneuver.type());

  if (maneuver.HasBeginStreetNames()) {
    instruction += " onto ";
    instruction += maneuver.begin_street_names().ToString(element_max_count,
                                                          delim);
  } else if (maneuver.HasStreetNames()) {
    instruction += " onto ";
    instruction += maneuver.street_names().ToString(element_max_count,
                                                    delim);
  }

  instruction += ".";
  return instruction;
}

std::string NarrativeBuilder::FormBearToStayOnInstruction(Maneuver& maneuver) {
  // "Bear <FormTurnTypeInstruction> to stay on <STREET_NAMES>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction += "Bear ";
  instruction += FormTurnTypeInstruction(maneuver.type());

  if (maneuver.HasStreetNames()) {
    instruction += " to stay on ";
    instruction += maneuver.street_names().ToString();
  }

  instruction += ".";
  return instruction;
}

std::string NarrativeBuilder::FormVerbalAlertBearToStayOnInstruction(
    Maneuver& maneuver, uint32_t element_max_count, std::string delim) {
  // "Bear <FormTurnTypeInstruction> to stay on <STREET_NAMES(1)>."

  return FormVerbalBearToStayOnInstruction(maneuver, element_max_count,
                                           delim);
}

std::string NarrativeBuilder::FormVerbalBearToStayOnInstruction(
    Maneuver& maneuver, uint32_t element_max_count, std::string delim) {
  // "Bear <FormTurnTypeInstruction> to stay on <STREET_NAMES(2)>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction += "Bear ";
  instruction += FormTurnTypeInstruction(maneuver.type());

  if (maneuver.HasStreetNames()) {
    instruction += " to stay on ";
    instruction += maneuver.street_names().ToString(element_max_count,
                                                    delim);
  }

  instruction += ".";
  return instruction;
}

std::string NarrativeBuilder::FormUturnInstruction(Maneuver& maneuver) {
  //  "Make a <FormTurnTypeInstruction> U-turn."
  //  "Make a <FormTurnTypeInstruction> U-turn at <CROSS_STREET_NAMES>."
  //  "Make a <FormTurnTypeInstruction> U-turn at <CROSS_STREET_NAMES> onto <STREET_NAMES>."
  //  "Make a <FormTurnTypeInstruction> U-turn onto <STREET_NAMES>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction += "Make a ";
  instruction += FormTurnTypeInstruction(maneuver.type());
  instruction += " U-turn";

  if (maneuver.HasCrossStreetNames()) {
    instruction += " at ";
    instruction += maneuver.cross_street_names().ToString();
  }

  if (maneuver.HasStreetNames()) {
    instruction += " onto ";
    instruction += maneuver.street_names().ToString();
  }

  instruction += ".";
  return instruction;
}

std::string NarrativeBuilder::FormVerbalAlertUturnInstruction(
    Maneuver& maneuver, uint32_t element_max_count, std::string delim) {
  //  "Make a <FormTurnTypeInstruction> U-turn."
  //  "Make a <FormTurnTypeInstruction> U-turn at <CROSS_STREET_NAMES(1)>."
  //  "Make a <FormTurnTypeInstruction> U-turn onto <STREET_NAMES(1)>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction += "Make a ";
  instruction += FormTurnTypeInstruction(maneuver.type());
  instruction += " U-turn";

  if (maneuver.HasCrossStreetNames()) {
    instruction += " at ";
    instruction += maneuver.cross_street_names().ToString(element_max_count,
                                                          delim);
  } else if (maneuver.HasStreetNames()) {
    instruction += " onto ";
    instruction += maneuver.street_names().ToString(element_max_count,
                                                    delim);
  }

  instruction += ".";
  return instruction;
}

std::string NarrativeBuilder::FormVerbalUturnInstruction(
    Maneuver& maneuver, uint32_t element_max_count, std::string delim) {
  //  "Make a <FormTurnTypeInstruction> U-turn."
  //  "Make a <FormTurnTypeInstruction> U-turn at <CROSS_STREET_NAMES(2)>."
  //  "Make a <FormTurnTypeInstruction> U-turn at <CROSS_STREET_NAMES(2)> onto <STREET_NAMES(2)>."
  //  "Make a <FormTurnTypeInstruction> U-turn onto <STREET_NAMES(2)>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction += "Make a ";
  instruction += FormTurnTypeInstruction(maneuver.type());
  instruction += " U-turn";

  if (maneuver.HasCrossStreetNames()) {
    instruction += " at ";
    instruction += maneuver.cross_street_names().ToString(element_max_count,
                                                          delim);
  }

  if (maneuver.HasStreetNames()) {
    instruction += " onto ";
    instruction += maneuver.street_names().ToString(element_max_count,
                                                    delim);
  }

  instruction += ".";
  return instruction;
}

std::string NarrativeBuilder::FormRampStraightInstruction(
    Maneuver& maneuver, bool limit_by_consecutive_count,
    uint32_t element_max_count) {

  // 0 "Stay straight to take the ramp."
  // 1 "Stay straight to take the <BRANCH_SIGN> ramp."
  // 2 "Stay straight to take the ramp toward <TOWARD_SIGN>."
  // 3 "Stay straight to take the <BRANCH_SIGN> ramp toward <TOWARD_SIGN>."
  // 4 "Stay straight to take the <NAME_SIGN> ramp."

  // Examples
  // 0 Stay straight to take the ramp
  // 1 Stay straight to take the I 95 South ramp
  // 2 Stay straight to take the ramp toward Baltimore
  // 3 Stay straight to take the I 95 South ramp toward Baltimore
  // 4 Stay straight to take the Gettysburg Pike ramp

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  uint8_t phrase_id = 0;

  // 1 = branch
  // 2 = toward
  // 4 = name (if no branch and toward)
  std::string exit_branch_sign;
  std::string exit_toward_sign;
  std::string exit_name_sign;
  if (maneuver.HasExitBranchSign()) {
    phrase_id += 1;
    exit_branch_sign = maneuver.signs().GetExitBranchString(
        element_max_count, limit_by_consecutive_count);
  }
  if (maneuver.HasExitTowardSign()) {
    phrase_id += 2;
    exit_toward_sign = maneuver.signs().GetExitTowardString(
        element_max_count, limit_by_consecutive_count);
  }
  if (maneuver.HasExitNameSign() && !maneuver.HasExitBranchSign()
      && !maneuver.HasExitTowardSign()) {
    phrase_id += 4;
    exit_name_sign = maneuver.signs().GetExitNameString(
        element_max_count, limit_by_consecutive_count);
  }

  switch (phrase_id) {
    // 1 "Stay straight to take the <BRANCH_SIGN> ramp."
    case 1: {
      instruction = (boost::format("Stay straight to take the %1% ramp.")
          % exit_branch_sign).str();
      break;
    }
    // 2 "Stay straight to take the ramp toward <TOWARD_SIGN>."
    case 2: {
      instruction = (boost::format("Stay straight to take the ramp toward %1%.")
          % exit_toward_sign).str();
      break;
    }
    // 3 "Stay straight to take the <BRANCH_SIGN> ramp toward <TOWARD_SIGN>."
    case 3: {
      instruction = (boost::format(
          "Stay straight to take the %1% ramp toward %2%.") % exit_branch_sign
          % exit_toward_sign).str();
      break;
    }
    // 4 "Stay straight to take the <NAME_SIGN> ramp."
    case 4: {
      instruction = (boost::format("Stay straight to take the %1% ramp.")
          % exit_name_sign).str();
      break;
    }
    default: {
      // 0 "Stay straight to take the ramp."
      instruction = "Stay straight to take the ramp.";
      break;
    }
  }

  return instruction;
}

std::string NarrativeBuilder::FormVerbalAlertRampStraightInstruction(
    Maneuver& maneuver, bool limit_by_consecutive_count,
    uint32_t element_max_count, std::string delim) {

  // 0 "Stay straight to take the ramp."
  // 1 "Stay straight to take the <BRANCH_SIGN(1)> ramp."
  // 2 "Stay straight to take the ramp toward <TOWARD_SIGN(1)>."
  // 3 "Stay straight to take the <NAME_SIGN(1)> ramp."

  // Examples
  // 0 Stay straight to take the ramp
  // 1 Stay straight to take the I 95 South ramp
  // 2 Stay straight to take the ramp toward Baltimore
  // 3 Stay straight to take the Gettysburg Pike ramp

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  uint8_t phrase_id = 0;

  // 1 = branch
  // 2 = toward
  // 3 = name (if no branch and toward)
  std::string exit_branch_sign;
  std::string exit_toward_sign;
  std::string exit_name_sign;
  if (maneuver.HasExitBranchSign()) {
    phrase_id = 1;
    exit_branch_sign = maneuver.signs().GetExitBranchString(
        element_max_count, limit_by_consecutive_count, delim);
  }
  else if (maneuver.HasExitTowardSign()) {
    phrase_id = 2;
    exit_toward_sign = maneuver.signs().GetExitTowardString(
        element_max_count, limit_by_consecutive_count, delim);
  }
  else if (maneuver.HasExitNameSign()) {
    phrase_id = 3;
    exit_name_sign = maneuver.signs().GetExitNameString(
        element_max_count, limit_by_consecutive_count, delim);
  }

  switch (phrase_id) {
    // 1 "Stay straight to take the <BRANCH_SIGN> ramp."
    case 1: {
      instruction = (boost::format("Stay straight to take the %1% ramp.")
          % exit_branch_sign).str();
      break;
    }
    // 2 "Stay straight to take the ramp toward <TOWARD_SIGN>."
    case 2: {
      instruction = (boost::format("Stay straight to take the ramp toward %1%.")
          % exit_toward_sign).str();
      break;
    }
    // 3 "Stay straight to take the <NAME_SIGN> ramp."
    case 3: {
      instruction = (boost::format("Stay straight to take the %1% ramp.")
          % exit_name_sign).str();
      break;
    }
    default: {
      // 0 "Stay straight to take the ramp."
      instruction = "Stay straight to take the ramp.";
      break;
    }
  }

  return instruction;
}

std::string NarrativeBuilder::FormVerbalRampStraightInstruction(
    Maneuver& maneuver, bool limit_by_consecutive_count,
    uint32_t element_max_count, std::string delim) {

  // 0 "Stay straight to take the ramp."
  // 1 "Stay straight to take the <BRANCH_SIGN(2)> ramp."
  // 2 "Stay straight to take the ramp toward <TOWARD_SIGN(2)>."
  // 3 "Stay straight to take the <BRANCH_SIGN(2)> ramp toward <TOWARD_SIGN(2)>."
  // 4 "Stay straight to take the <NAME_SIGN(2)> ramp."

  // Examples
  // 0 Stay straight to take the ramp
  // 1 Stay straight to take the I 95 South ramp
  // 2 Stay straight to take the ramp toward Baltimore
  // 3 Stay straight to take the I 95 South ramp toward Baltimore
  // 4 Stay straight to take the Gettysburg Pike ramp

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  uint8_t phrase_id = 0;

  // 1 = branch
  // 2 = toward
  // 4 = name (if no branch and toward)
  std::string exit_branch_sign;
  std::string exit_toward_sign;
  std::string exit_name_sign;
  if (maneuver.HasExitBranchSign()) {
    phrase_id += 1;
    exit_branch_sign = maneuver.signs().GetExitBranchString(
        element_max_count, limit_by_consecutive_count, delim);
  }
  if (maneuver.HasExitTowardSign()) {
    phrase_id += 2;
    exit_toward_sign = maneuver.signs().GetExitTowardString(
        element_max_count, limit_by_consecutive_count, delim);
  }
  if (maneuver.HasExitNameSign() && !maneuver.HasExitBranchSign()
      && !maneuver.HasExitTowardSign()) {
    phrase_id += 4;
    exit_name_sign = maneuver.signs().GetExitNameString(
        element_max_count, limit_by_consecutive_count, delim);
  }

  switch (phrase_id) {
    // 1 "Stay straight to take the <BRANCH_SIGN> ramp."
    case 1: {
      instruction = (boost::format("Stay straight to take the %1% ramp.")
          % exit_branch_sign).str();
      break;
    }
    // 2 "Stay straight to take the ramp toward <TOWARD_SIGN>."
    case 2: {
      instruction = (boost::format("Stay straight to take the ramp toward %1%.")
          % exit_toward_sign).str();
      break;
    }
    // 3 "Stay straight to take the <BRANCH_SIGN> ramp toward <TOWARD_SIGN>."
    case 3: {
      instruction = (boost::format(
          "Stay straight to take the %1% ramp toward %2%.") % exit_branch_sign
          % exit_toward_sign).str();
      break;
    }
    // 4 "Stay straight to take the <NAME_SIGN> ramp."
    case 4: {
      instruction = (boost::format("Stay straight to take the %1% ramp.")
          % exit_name_sign).str();
      break;
    }
    default: {
      // 0 "Stay straight to take the ramp."
      instruction = "Stay straight to take the ramp.";
      break;
    }
  }

  return instruction;
}

std::string NarrativeBuilder::FormRampInstruction(
    Maneuver& maneuver, bool limit_by_consecutive_count,
    uint32_t element_max_count) {
  // 0 "Take the ramp on the <FormTurnTypeInstruction>."
  // 1 "Take the <BRANCH_SIGN> ramp on the <FormTurnTypeInstruction>."
  // 2 "Take the ramp on the <FormTurnTypeInstruction> toward <TOWARD_SIGN>."
  // 3 "Take the <BRANCH_SIGN> ramp on the <FormTurnTypeInstruction> toward <TOWARD_SIGN>."
  // 4 "Take the <NAME_SIGN> ramp on the <FormTurnTypeInstruction>."
  // 5 "Turn <FormTurnTypeInstruction> to take the ramp."
  // 6 "Turn <FormTurnTypeInstruction> to take the <BRANCH_SIGN> ramp."
  // 7 "Turn <FormTurnTypeInstruction> to take the ramp toward <TOWARD_SIGN>."
  // 8 "Turn <FormTurnTypeInstruction> to take the <BRANCH_SIGN> ramp toward <TOWARD_SIGN>."
  // 9 "Turn <FormTurnTypeInstruction> to take the <NAME_SIGN> ramp."

  // Examples
  // 0 Take the ramp on the right
  // 1 Take the I 95 South ramp on the right
  // 2 Take the ramp on the right toward Baltimore
  // 3 Take the I 95 South ramp on the right toward Baltimore
  // 4 Take the Gettysburg Pike ramp on the right
  // 5 Turn right to take the ramp
  // 6 Turn right to take the I 95 South ramp
  // 7 Turn right to take the ramp toward Baltimore
  // 8 Turn right to take the I 95 South ramp toward Baltimore
  // 9 Turn right to take the Gettysburg Pike ramp

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);

  // 0 = Take the ramp on the right
  // 1 = branch
  // 2 = toward
  // 4 = name (if no branch and toward)
  // 5 = Turn right to take the ramp
  uint8_t phrase_id = 0;

  if ((maneuver.begin_relative_direction() == Maneuver::RelativeDirection::kRight)
      || (maneuver.begin_relative_direction() == Maneuver::RelativeDirection::kLeft))
    phrase_id = 5;

  std::string turn = FormTurnTypeInstruction(maneuver.type());
  std::string exit_branch_sign;
  std::string exit_toward_sign;
  std::string exit_name_sign;
  if (maneuver.HasExitBranchSign()) {
    phrase_id += 1;
    exit_branch_sign = maneuver.signs().GetExitBranchString(
        element_max_count, limit_by_consecutive_count);
  }
  if (maneuver.HasExitTowardSign()) {
    phrase_id += 2;
    exit_toward_sign = maneuver.signs().GetExitTowardString(
        element_max_count, limit_by_consecutive_count);
  }
  if (maneuver.HasExitNameSign() && !maneuver.HasExitBranchSign()
      && !maneuver.HasExitTowardSign()) {
    phrase_id += 4;
    exit_name_sign = maneuver.signs().GetExitNameString(
        element_max_count, limit_by_consecutive_count);
  }

  switch (phrase_id) {
    // 1 "Take the <BRANCH_SIGN> ramp on the <FormTurnTypeInstruction>."
    case 1: {
      instruction = (boost::format("Take the %1% ramp on the %2%.")
          % exit_branch_sign % turn).str();
      break;
    }
      // 2 "Take the ramp on the <FormTurnTypeInstruction> toward <TOWARD_SIGN>."
    case 2: {
      instruction = (boost::format("Take the ramp on the %1% toward %2%.")
          % turn % exit_toward_sign).str();
      break;
    }
      // 3 "Take the <BRANCH_SIGN> ramp on the <FormTurnTypeInstruction> toward <TOWARD_SIGN>."
    case 3: {
      instruction = (boost::format("Take the %1% ramp on the %2% toward %3%.")
          % exit_branch_sign % turn % exit_toward_sign).str();
      break;
    }
      // 4 "Take the <NAME_SIGN> ramp on the <FormTurnTypeInstruction>."
    case 4: {
      instruction = (boost::format("Take the %1% ramp on the %2%.")
          % exit_name_sign % turn).str();
      break;
    }
      // 5 "Turn <FormTurnTypeInstruction> to take the ramp."
    case 5: {
      instruction = (boost::format("Turn %1% to take the ramp.") % turn).str();
      break;
    }
      // 6 "Turn <FormTurnTypeInstruction> to take the <BRANCH_SIGN> ramp."
    case 6: {
      instruction = (boost::format("Turn %1% to take the %2% ramp.")
          % turn % exit_branch_sign).str();
      break;
    }
      // 7 "Turn <FormTurnTypeInstruction> to take the ramp toward <TOWARD_SIGN>."
    case 7: {
      instruction = (boost::format("Turn %1% to take the ramp toward %2%.")
          % turn % exit_toward_sign).str();
      break;
    }
      // 8 "Turn <FormTurnTypeInstruction> to take the <BRANCH_SIGN> ramp toward <TOWARD_SIGN>."
    case 8: {
      instruction = (boost::format("Turn %1% to take the %2% ramp toward %3%.")
          % turn % exit_branch_sign % exit_toward_sign).str();
      break;
    }
      // 9 "Turn <FormTurnTypeInstruction> to take the <NAME_SIGN> ramp."
    case 9: {
      instruction = (boost::format("Turn %1% to take the %2% ramp.")
          % turn % exit_name_sign).str();
      break;
    }
      // 0 "Take the ramp on the <FormTurnTypeInstruction>."
    default: {
      instruction = (boost::format("Take the ramp on the %1%.") % turn).str();
      break;
    }
  }

  return instruction;
}

std::string NarrativeBuilder::FormVerbalAlertRampInstruction(
    Maneuver& maneuver, bool limit_by_consecutive_count,
    uint32_t element_max_count, std::string delim) {
  // 0 "Take the ramp on the <FormTurnTypeInstruction>."
  // 1 "Take the <BRANCH_SIGN(1)> ramp on the <FormTurnTypeInstruction>."
  // 2 "Take the ramp on the <FormTurnTypeInstruction> toward <TOWARD_SIGN(1)>."
  // 4 "Take the <NAME_SIGN(1)> ramp on the <FormTurnTypeInstruction>."
  // 5 "Turn <FormTurnTypeInstruction> to take the ramp."
  // 6 "Turn <FormTurnTypeInstruction> to take the <BRANCH_SIGN(1)> ramp."
  // 7 "Turn <FormTurnTypeInstruction> to take the ramp toward <TOWARD_SIGN(1)>."
  // 9 "Turn <FormTurnTypeInstruction> to take the <NAME_SIGN(1)> ramp."

  // 0 = Take the ramp on the right
  // 1 = branch
  // 2 = toward
  // 4 = name (if no branch and toward)
  // 5 = Turn right to take the ramp
  uint8_t phrase_id = 0;
  std::string exit_branch_sign;
  std::string exit_toward_sign;
  std::string exit_name_sign;
  if ((maneuver.begin_relative_direction() == Maneuver::RelativeDirection::kRight)
      || (maneuver.begin_relative_direction() == Maneuver::RelativeDirection::kLeft))
    phrase_id = 5;

  if (maneuver.HasExitBranchSign()) {
    phrase_id += 1;
    exit_branch_sign = maneuver.signs().GetExitBranchString(
        element_max_count, limit_by_consecutive_count, delim);
  } else if (maneuver.HasExitTowardSign()) {
    phrase_id += 2;
    exit_toward_sign = maneuver.signs().GetExitTowardString(
        element_max_count, limit_by_consecutive_count, delim);
  } else if (maneuver.HasExitNameSign()) {
    phrase_id += 4;
    exit_name_sign = maneuver.signs().GetExitNameString(
        element_max_count, limit_by_consecutive_count, delim);
  }

  return FormVerbalRampInstruction(phrase_id,
                                   FormTurnTypeInstruction(maneuver.type()),
                                   exit_branch_sign, exit_toward_sign,
                                   exit_name_sign);
}

std::string NarrativeBuilder::FormVerbalRampInstruction(
    Maneuver& maneuver, bool limit_by_consecutive_count,
    uint32_t element_max_count, std::string delim) {
  // 0 "Take the ramp on the <FormTurnTypeInstruction>."
  // 1 "Take the <BRANCH_SIGN(2)> ramp on the <FormTurnTypeInstruction>."
  // 2 "Take the ramp on the <FormTurnTypeInstruction> toward <TOWARD_SIGN(2)>."
  // 3 "Take the <BRANCH_SIGN(2)> ramp on the <FormTurnTypeInstruction> toward <TOWARD_SIGN(2)>."
  // 4 "Take the <NAME_SIGN(2)> ramp on the <FormTurnTypeInstruction>."
  // 5 "Turn <FormTurnTypeInstruction> to take the ramp."
  // 6 "Turn <FormTurnTypeInstruction> to take the <BRANCH_SIGN(2)> ramp."
  // 7 "Turn <FormTurnTypeInstruction> to take the ramp toward <TOWARD_SIGN(2)>."
  // 8 "Turn <FormTurnTypeInstruction> to take the <BRANCH_SIGN(2)> ramp toward <TOWARD_SIGN(2)>."
  // 9 "Turn <FormTurnTypeInstruction> to take the <NAME_SIGN(2)> ramp."


  // 0 = Take the ramp on the right
  // 1 = branch
  // 2 = toward
  // 4 = name (if no branch and toward)
  // 5 = Turn right to take the ramp
  uint8_t phrase_id = 0;
  std::string exit_branch_sign;
  std::string exit_toward_sign;
  std::string exit_name_sign;
  if ((maneuver.begin_relative_direction() == Maneuver::RelativeDirection::kRight)
      || (maneuver.begin_relative_direction() == Maneuver::RelativeDirection::kLeft))
    phrase_id = 5;

  if (maneuver.HasExitBranchSign()) {
    phrase_id += 1;
    exit_branch_sign = maneuver.signs().GetExitBranchString(
        element_max_count, limit_by_consecutive_count, delim);
  }
  if (maneuver.HasExitTowardSign()) {
    phrase_id += 2;
    exit_toward_sign = maneuver.signs().GetExitTowardString(
        element_max_count, limit_by_consecutive_count, delim);
  }
  if (maneuver.HasExitNameSign() && !maneuver.HasExitBranchSign()
      && !maneuver.HasExitTowardSign()) {
    phrase_id += 4;
    exit_name_sign = maneuver.signs().GetExitNameString(
        element_max_count, limit_by_consecutive_count, delim);
  }

  return FormVerbalRampInstruction(phrase_id,
                                   FormTurnTypeInstruction(maneuver.type()),
                                   exit_branch_sign, exit_toward_sign,
                                   exit_name_sign);
}

std::string NarrativeBuilder::FormVerbalRampInstruction(
    uint8_t phrase_id, const std::string& turn,
    const std::string& exit_branch_sign, const std::string& exit_toward_sign,
    const std::string& exit_name_sign) {

  // Examples
  // 0 Take the ramp on the right
  // 1 Take the I 95 South ramp on the right
  // 2 Take the ramp on the right toward Baltimore
  // 3 Take the I 95 South ramp on the right toward Baltimore
  // 4 Take the Gettysburg Pike ramp on the right
  // 5 Turn right to take the ramp
  // 6 Turn right to take the I 95 South ramp
  // 7 Turn right to take the ramp toward Baltimore
  // 8 Turn right to take the I 95 South ramp toward Baltimore
  // 9 Turn right to take the Gettysburg Pike ramp

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);

  switch (phrase_id) {
    // 1 "Take the <BRANCH_SIGN> ramp on the <FormTurnTypeInstruction>."
    case 1: {
      instruction = (boost::format("Take the %1% ramp on the %2%.")
          % exit_branch_sign % turn).str();
      break;
    }
      // 2 "Take the ramp on the <FormTurnTypeInstruction> toward <TOWARD_SIGN>."
    case 2: {
      instruction = (boost::format("Take the ramp on the %1% toward %2%.")
          % turn % exit_toward_sign).str();
      break;
    }
      // 3 "Take the <BRANCH_SIGN> ramp on the <FormTurnTypeInstruction> toward <TOWARD_SIGN>."
    case 3: {
      instruction = (boost::format("Take the %1% ramp on the %2% toward %3%.")
          % exit_branch_sign % turn % exit_toward_sign).str();
      break;
    }
      // 4 "Take the <NAME_SIGN> ramp on the <FormTurnTypeInstruction>."
    case 4: {
      instruction = (boost::format("Take the %1% ramp on the %2%.")
          % exit_name_sign % turn).str();
      break;
    }
      // 5 "Turn <FormTurnTypeInstruction> to take the ramp."
    case 5: {
      instruction = (boost::format("Turn %1% to take the ramp.") % turn).str();
      break;
    }
      // 6 "Turn <FormTurnTypeInstruction> to take the <BRANCH_SIGN> ramp."
    case 6: {
      instruction = (boost::format("Turn %1% to take the %2% ramp.")
          % turn % exit_branch_sign).str();
      break;
    }
      // 7 "Turn <FormTurnTypeInstruction> to take the ramp toward <TOWARD_SIGN>."
    case 7: {
      instruction = (boost::format("Turn %1% to take the ramp toward %2%.")
          % turn % exit_toward_sign).str();
      break;
    }
      // 8 "Turn <FormTurnTypeInstruction> to take the <BRANCH_SIGN> ramp toward <TOWARD_SIGN>."
    case 8: {
      instruction = (boost::format("Turn %1% to take the %2% ramp toward %3%.")
          % turn % exit_branch_sign % exit_toward_sign).str();
      break;
    }
      // 9 "Turn <FormTurnTypeInstruction> to take the <NAME_SIGN> ramp."
    case 9: {
      instruction = (boost::format("Turn %1% to take the %2% ramp.")
          % turn % exit_name_sign).str();
      break;
    }
      // 0 "Take the ramp on the <FormTurnTypeInstruction>."
    default: {
      instruction = (boost::format("Take the ramp on the %1%.") % turn).str();
      break;
    }
  }

  return instruction;
}

std::string NarrativeBuilder::FormExitInstruction(Maneuver& maneuver,
                                           bool limit_by_consecutive_count,
                                           uint32_t element_max_count) {
  //  0 "Take the exit on the right"
  //  1 "Take exit <NUMBER_SIGN> on the <FormTurnTypeInstruction>."
  //  2 "Take the <BRANCH_SIGN> exit on the <FormTurnTypeInstruction>."
  //  3 "Take exit <NUMBER_SIGN> on the <FormTurnTypeInstruction> onto <BRANCH_SIGN>."
  //  4 "Take the exit on the <FormTurnTypeInstruction> toward <TOWARD_SIGN>."
  //  5 "Take exit <NUMBER_SIGN> on the <FormTurnTypeInstruction> toward <TOWARD_SIGN>."
  //  6 "Take the <BRANCH_SIGN> exit on the <FormTurnTypeInstruction> toward <TOWARD_SIGN>."
  //  7 "Take exit <NUMBER_SIGN> on the <FormTurnTypeInstruction> onto <BRANCH_SIGN> toward <TOWARD_SIGN>."
  //  8 "Take the <NAME_SIGN> exit on the <FormTurnTypeInstruction>."
  //  10 "Take the <NAME_SIGN> exit on the <FormTurnTypeInstruction> onto <BRANCH_SIGN>."
  //  12 "Take the <NAME_SIGN> exit on the <FormTurnTypeInstruction> toward <TOWARD_SIGN>."
  //  14 "Take the <NAME_SIGN> exit on the <FormTurnTypeInstruction> onto <BRANCH_SIGN> toward <TOWARD_SIGN>."

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);

  // 0 Take the exit on the right
  // 1 = exit number
  // 2 = branch
  // 4 = toward
  // 8 = name (when no number)
  uint8_t phrase_id = 0;
  std::string turn = FormTurnTypeInstruction(maneuver.type());
  std::string exit_number_sign;
  std::string exit_branch_sign;
  std::string exit_toward_sign;
  std::string exit_name_sign;
  if (maneuver.HasExitNumberSign()) {
    phrase_id += 1;
    exit_number_sign = maneuver.signs().GetExitNumberString();
  }
  if (maneuver.HasExitBranchSign()) {
    phrase_id += 2;
    exit_branch_sign = maneuver.signs().GetExitBranchString(
        element_max_count, limit_by_consecutive_count);
  }
  if (maneuver.HasExitTowardSign()) {
    phrase_id += 4;
    exit_toward_sign = maneuver.signs().GetExitTowardString(
        element_max_count, limit_by_consecutive_count);
  }
  if (maneuver.HasExitNameSign() && !maneuver.HasExitNumberSign()) {
    phrase_id += 8;
    exit_name_sign = maneuver.signs().GetExitNameString(
        element_max_count, limit_by_consecutive_count);
  }

  switch (phrase_id) {
    //  1 "Take exit <NUMBER_SIGN> on the <FormTurnTypeInstruction>."
    case 1: {
      instruction = (boost::format("Take exit %1% on the %2%.")
          % exit_number_sign % turn).str();
      break;
    }
      //  2 "Take the <BRANCH_SIGN> exit on the <FormTurnTypeInstruction>."
    case 2: {
      instruction = (boost::format("Take the %1% exit on the %2%.")
          % exit_branch_sign % turn).str();
      break;
    }
      //  3 "Take exit <NUMBER_SIGN> on the <FormTurnTypeInstruction> onto <BRANCH_SIGN>."
    case 3: {
      instruction = (boost::format("Take exit %1% on the %2% onto %3%.")
          % exit_number_sign % turn % exit_branch_sign).str();
      break;
    }
      //  4 "Take the exit on the <FormTurnTypeInstruction> toward <TOWARD_SIGN>."
    case 4: {
      instruction = (boost::format("Take the exit on the %1% toward %2%.")
          % turn % exit_toward_sign).str();
      break;
    }
      //  5 "Take exit <NUMBER_SIGN> on the <FormTurnTypeInstruction> toward <TOWARD_SIGN>."
    case 5: {
      instruction = (boost::format("Take exit %1% on the %2% toward %3%.")
          % exit_number_sign % turn % exit_toward_sign).str();
      break;
    }
      //  6 "Take the <BRANCH_SIGN> exit on the <FormTurnTypeInstruction> toward <TOWARD_SIGN>."
    case 6: {
      instruction = (boost::format("Take the %1% exit on the %2% toward %3%.")
          % exit_branch_sign % turn % exit_toward_sign).str();
      break;
    }
      //  7 "Take exit <NUMBER_SIGN> on the <FormTurnTypeInstruction> onto <BRANCH_SIGN> toward <TOWARD_SIGN>."
    case 7: {
      instruction = (boost::format(
          "Take exit %1% on the %2% onto %3% toward %4%.") % exit_number_sign
          % turn % exit_branch_sign % exit_toward_sign).str();
      break;
    }
      //  8 "Take the <NAME_SIGN> exit on the <FormTurnTypeInstruction>."
    case 8: {
      instruction = (boost::format("Take the %1% exit on the %2%.")
          % exit_name_sign % turn).str();
      break;
    }
      //  10 "Take the <NAME_SIGN> exit on the <FormTurnTypeInstruction> onto <BRANCH_SIGN>."
    case 10: {
      instruction = (boost::format("Take the %1% exit on the %2% onto %3%.")
          % exit_name_sign % turn % exit_branch_sign).str();
      break;
    }
      //  12 "Take the <NAME_SIGN> exit on the <FormTurnTypeInstruction> toward <TOWARD_SIGN>."
    case 12: {
      instruction = (boost::format("Take the %1% exit on the %2% toward %3%.")
          % exit_name_sign % turn % exit_toward_sign).str();
      break;
    }
      //  14 "Take the <NAME_SIGN> exit on the <FormTurnTypeInstruction> onto <BRANCH_SIGN> toward <TOWARD_SIGN>."
    case 14: {
      instruction = (boost::format(
          "Take the %1% exit on the %2% onto %3% toward %4%.") % exit_name_sign
          % turn % exit_branch_sign % exit_toward_sign).str();
      break;
    }
    default: {
      //  0 "Take the exit on the right"
      instruction = (boost::format("Take the exit on the %1%.") % turn).str();
      break;
    }
  }

  return instruction;
}

void NarrativeBuilder::FormStayStraightInstruction(Maneuver& maneuver) {
  // 1 Keep straight to take exit 67A
  // 2 Keep straight to take I 95 South
  // 3 Keep straight to take exit 67A onto I 95 South
  // 4 Keep straight toward Baltimore
  // 5 Keep straight to take exit 67A toward Baltimore
  // 6 Keep straight to take I 95 South toward Baltimore
  // 7 Keep straight to take exit 67A onto I 95 South toward Baltimore

  // 0 Keep straight at fork
  // 1 = exit number
  // 2 = street or branch
  // 4 = toward

  // TODO: determine if we want to grab from options
  uint32_t number_max_count = 0;
  uint32_t branch_max_count = 4;
  uint32_t toward_max_count = 4;
  bool number_limit_by_consecutive_count = false;
  bool branch_limit_by_consecutive_count = true;
  bool toward_limit_by_consecutive_count = true;

  // Assign maneuver street name or sign branch name
  std::string street_name;
  if (maneuver.HasStreetNames()) {
    street_name = maneuver.street_names().ToString();
  } else if (maneuver.HasExitBranchSign()) {
    street_name = maneuver.signs().GetExitBranchString(
        branch_max_count, branch_limit_by_consecutive_count);
  }

  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  uint8_t phrase_id = 0;
  if (maneuver.HasExitNumberSign())
    phrase_id += 1;
  if (!street_name.empty())
    phrase_id += 2;
  if (maneuver.HasExitTowardSign())
    phrase_id += 4;

  switch (phrase_id) {
    // 1 Keep straight to take exit 67A
    case 1: {
      text_instruction += (boost::format("Keep straight to take exit %1%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)).str();
      break;
    }
    // 2 Keep straight to take I 95 South
    case 2: {
      text_instruction +=
          (boost::format("Keep straight to take %1%") % street_name).str();
      break;
    }
    // 3 Keep straight to take exit 67A onto I 95 South
    case 3: {
      text_instruction += (boost::format("Keep straight to take exit %1% onto %2%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)
          % street_name).str();
      break;
    }
    // 4 Keep straight toward Baltimore
    case 4: {
      text_instruction += (boost::format(
          "Keep straight toward %1%")
          % maneuver.signs().GetExitTowardString(toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    // 5 Keep straight to take exit 67A toward Baltimore
    case 5: {
      text_instruction += (boost::format(
          "Keep straight to take exit %1% toward %2%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)
          % maneuver.signs().GetExitTowardString(toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    // 6 Keep straight to take I 95 South toward Baltimore
    case 6: {
      text_instruction += (boost::format(
          "Keep straight to take %1% toward %2%")
          % street_name
          % maneuver.signs().GetExitTowardString(toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    // 7 Keep straight to take exit 67A onto I 95 South toward Baltimore
    case 7: {
      text_instruction += (boost::format(
          "Keep straight to take exit %1% onto %2% toward %3%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)
          % street_name
          % maneuver.signs().GetExitTowardString(toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    default: {
      text_instruction += "Keep straight at the fork";
      break;
    }
  }

  text_instruction += ".";
  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormStayRightInstruction(Maneuver& maneuver) {
  // 1 Keep right to take exit 67A
  // 2 Keep right to take I 95 South
  // 3 Keep right to take exit 67A onto I 95 South
  // 4 Keep right toward Baltimore
  // 5 Keep right to take exit 67A toward Baltimore
  // 6 Keep right to take I 95 South toward Baltimore
  // 7 Keep right to take exit 67A onto I 95 South toward Baltimore

  // 0 Keep right at fork
  // 1 = exit number
  // 2 = street or branch
  // 4 = toward

  // TODO: determine if we want to grab from options
  uint32_t number_max_count = 0;
  uint32_t branch_max_count = 4;
  uint32_t toward_max_count = 4;
  bool number_limit_by_consecutive_count = false;
  bool branch_limit_by_consecutive_count = true;
  bool toward_limit_by_consecutive_count = true;

  // Assign maneuver street name or sign branch name
  std::string street_name;
  if (maneuver.HasStreetNames()) {
    street_name = maneuver.street_names().ToString();
  } else if (maneuver.HasExitBranchSign()) {
    street_name = maneuver.signs().GetExitBranchString(
        branch_max_count, branch_limit_by_consecutive_count);
  }

  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  uint8_t phrase_id = 0;
  if (maneuver.HasExitNumberSign())
    phrase_id += 1;
  if (!street_name.empty())
    phrase_id += 2;
  if (maneuver.HasExitTowardSign())
    phrase_id += 4;

  switch (phrase_id) {
    // 1 Keep right to take exit 67A
    case 1: {
      text_instruction += (boost::format("Keep right to take exit %1%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)).str();
      break;
    }
    // 2 Keep right to take I 95 South
    case 2: {
      text_instruction +=
          (boost::format("Keep right to take %1%") % street_name).str();
      break;
    }
    // 3 Keep right to take exit 67A onto I 95 South
    case 3: {
      text_instruction += (boost::format("Keep right to take exit %1% onto %2%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)
          % street_name).str();
      break;
    }
    // 4 Keep right toward Baltimore
    case 4: {
      text_instruction += (boost::format(
          "Keep right toward %1%")
          % maneuver.signs().GetExitTowardString(toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    // 5 Keep right to take exit 67A toward Baltimore
    case 5: {
      text_instruction += (boost::format(
          "Keep right to take exit %1% toward %2%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)
          % maneuver.signs().GetExitTowardString(toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    // 6 Keep right to take I 95 South toward Baltimore
    case 6: {
      text_instruction += (boost::format(
          "Keep right to take %1% toward %2%")
          % street_name
          % maneuver.signs().GetExitTowardString(toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    // 7 Keep right to take exit 67A onto I 95 South toward Baltimore
    case 7: {
      text_instruction += (boost::format(
          "Keep right to take exit %1% onto %2% toward %3%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)
          % street_name
          % maneuver.signs().GetExitTowardString(toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    default: {
      text_instruction += "Keep right at the fork";
      break;
    }
  }

  text_instruction += ".";
  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormStayLeftInstruction(Maneuver& maneuver) {
  // 1 Keep left to take exit 67A
  // 2 Keep left to take I 95 South
  // 3 Keep left to take exit 67A onto I 95 South
  // 4 Keep left toward Baltimore
  // 5 Keep left to take exit 67A toward Baltimore
  // 6 Keep left to take I 95 South toward Baltimore
  // 7 Keep left to take exit 67A onto I 95 South toward Baltimore

  // 0 Keep left at fork
  // 1 = exit number
  // 2 = street or branch
  // 4 = toward

  // TODO: determine if we want to grab from options
  uint32_t number_max_count = 0;
  uint32_t branch_max_count = 4;
  uint32_t toward_max_count = 4;
  bool number_limit_by_consecutive_count = false;
  bool branch_limit_by_consecutive_count = true;
  bool toward_limit_by_consecutive_count = true;

  // Assign maneuver street name or sign branch name
  std::string street_name;
  if (maneuver.HasStreetNames()) {
    street_name = maneuver.street_names().ToString();
  } else if (maneuver.HasExitBranchSign()) {
    street_name = maneuver.signs().GetExitBranchString(
        branch_max_count, branch_limit_by_consecutive_count);
  }

  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  uint8_t phrase_id = 0;
  if (maneuver.HasExitNumberSign())
    phrase_id += 1;
  if (!street_name.empty())
    phrase_id += 2;
  if (maneuver.HasExitTowardSign())
    phrase_id += 4;

  switch (phrase_id) {
    // 1 Keep left to take exit 67A
    case 1: {
      text_instruction += (boost::format("Keep left to take exit %1%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)).str();
      break;
    }
    // 2 Keep left to take I 95 South
    case 2: {
      text_instruction +=
          (boost::format("Keep left to take %1%") % street_name).str();
      break;
    }
    // 3 Keep left to take exit 67A onto I 95 South
    case 3: {
      text_instruction += (boost::format("Keep left to take exit %1% onto %2%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)
          % street_name).str();
      break;
    }
    // 4 Keep left toward Baltimore
    case 4: {
      text_instruction += (boost::format(
          "Keep left toward %1%")
          % maneuver.signs().GetExitTowardString(toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    // 5 Keep left to take exit 67A toward Baltimore
    case 5: {
      text_instruction += (boost::format(
          "Keep left to take exit %1% toward %2%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)
          % maneuver.signs().GetExitTowardString(toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    // 6 Keep left to take I 95 South toward Baltimore
    case 6: {
      text_instruction += (boost::format(
          "Keep left to take %1% toward %2%")
          % street_name
          % maneuver.signs().GetExitTowardString(toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    // 7 Keep left to take exit 67A onto I 95 South toward Baltimore
    case 7: {
      text_instruction += (boost::format(
          "Keep left to take exit %1% onto %2% toward %3%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)
          % street_name
          % maneuver.signs().GetExitTowardString(toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    default: {
      text_instruction += "Keep left at the fork";
      break;
    }
  }

  text_instruction += ".";
  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormStayStraightToStayOnInstruction(Maneuver& maneuver) {
  // 0 Keep straight to stay on I 95 South
  // 1 Keep straight to take exit 67A to stay on I 95 South
  // 2 Keep straight to stay on I 95 South toward Baltimore
  // 3 Keep straight to take exit 67A to stay on I 95 South toward Baltimore

  // 0 = street
  // 1 = exit number
  // 2 = toward

  // TODO: determine if we want to grab from options
  uint32_t number_max_count = 0;
  uint32_t toward_max_count = 4;
  bool number_limit_by_consecutive_count = false;
  bool toward_limit_by_consecutive_count = true;

  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  uint8_t phrase_id = 0;
  if (maneuver.HasExitNumberSign())
    phrase_id += 1;
  if (maneuver.HasExitTowardSign())
    phrase_id += 2;

  switch (phrase_id) {
    // 1 Keep straight to take exit 67A to stay on I 95 South
    case 1: {
      text_instruction += (boost::format("Keep straight to take exit %1% to stay on %2%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)
          % maneuver.street_names().ToString()).str();
      break;
    }
    // 2 Keep straight to stay on I 95 South toward Baltimore
    case 2: {
      text_instruction += (boost::format("Keep straight to stay on %1% toward %2%")
          % maneuver.street_names().ToString()
          % maneuver.signs().GetExitTowardString(
              toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    // 3 Keep straight to take exit 67A to stay on I 95 South toward Baltimore
    case 3: {
      text_instruction += (boost::format("Keep straight to take exit %1% to stay on %2% toward %3%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)
          % maneuver.street_names().ToString()
          % maneuver.signs().GetExitTowardString(
              toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    default: {
      text_instruction += (boost::format("Keep straight to stay on %1%")
          % maneuver.street_names().ToString()).str();
      break;
    }
  }

  text_instruction += ".";
  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormStayRightToStayOnInstruction(Maneuver& maneuver) {
  // 0 Keep right to stay on I 95 South
  // 1 Keep right to take exit 67A to stay on I 95 South
  // 2 Keep right to stay on I 95 South toward Baltimore
  // 3 Keep right to take exit 67A to stay on I 95 South toward Baltimore

  // 0 = street
  // 1 = exit number
  // 2 = toward

  // TODO: determine if we want to grab from options
  uint32_t number_max_count = 0;
  uint32_t toward_max_count = 4;
  bool number_limit_by_consecutive_count = false;
  bool toward_limit_by_consecutive_count = true;

  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  uint8_t phrase_id = 0;
  if (maneuver.HasExitNumberSign())
    phrase_id += 1;
  if (maneuver.HasExitTowardSign())
    phrase_id += 2;

  switch (phrase_id) {
    // 1 Keep right to take exit 67A to stay on I 95 South
    case 1: {
      text_instruction += (boost::format("Keep right to take exit %1% to stay on %2%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)
          % maneuver.street_names().ToString()).str();
      break;
    }
    // 2 Keep right to stay on I 95 South toward Baltimore
    case 2: {
      text_instruction += (boost::format("Keep right to stay on %1% toward %2%")
          % maneuver.street_names().ToString()
          % maneuver.signs().GetExitTowardString(
              toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    // 3 Keep right to take exit 67A to stay on I 95 South toward Baltimore
    case 3: {
      text_instruction += (boost::format("Keep right to take exit %1% to stay on %2% toward %3%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)
          % maneuver.street_names().ToString()
          % maneuver.signs().GetExitTowardString(
              toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    default: {
      text_instruction += (boost::format("Keep right to stay on %1%")
          % maneuver.street_names().ToString()).str();
      break;
    }
  }

  text_instruction += ".";
  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormStayLeftToStayOnInstruction(Maneuver& maneuver) {
  // 0 Keep left to stay on I 95 South
  // 1 Keep left to take exit 67A to stay on I 95 South
  // 2 Keep left to stay on I 95 South toward Baltimore
  // 3 Keep left to take exit 67A to stay on I 95 South toward Baltimore

  // 0 = street
  // 1 = exit number
  // 2 = toward

  // TODO: determine if we want to grab from options
  uint32_t number_max_count = 0;
  uint32_t toward_max_count = 4;
  bool number_limit_by_consecutive_count = false;
  bool toward_limit_by_consecutive_count = true;

  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  uint8_t phrase_id = 0;
  if (maneuver.HasExitNumberSign())
    phrase_id += 1;
  if (maneuver.HasExitTowardSign())
    phrase_id += 2;

  switch (phrase_id) {
    // 1 Keep left to take exit 67A to stay on I 95 South
    case 1: {
      text_instruction += (boost::format("Keep left to take exit %1% to stay on %2%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)
          % maneuver.street_names().ToString()).str();
      break;
    }
    // 2 Keep left to stay on I 95 South toward Baltimore
    case 2: {
      text_instruction += (boost::format("Keep left to stay on %1% toward %2%")
          % maneuver.street_names().ToString()
          % maneuver.signs().GetExitTowardString(
              toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    // 3 Keep left to take exit 67A to stay on I 95 South toward Baltimore
    case 3: {
      text_instruction += (boost::format("Keep left to take exit %1% to stay on %2% toward %3%")
          % maneuver.signs().GetExitNumberString(number_max_count, number_limit_by_consecutive_count)
          % maneuver.street_names().ToString()
          % maneuver.signs().GetExitTowardString(
              toward_max_count, toward_limit_by_consecutive_count)).str();
      break;
    }
    default: {
      text_instruction += (boost::format("Keep left to stay on %1%")
          % maneuver.street_names().ToString()).str();
      break;
    }
  }

  text_instruction += ".";
  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormMergeInstruction(Maneuver& maneuver) {
  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  if (maneuver.HasStreetNames()) {
    text_instruction += "Merge onto ";
    text_instruction += maneuver.street_names().ToString();
  } else
    text_instruction += "Merge";

  text_instruction += ".";
  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormEnterRoundaboutInstruction(Maneuver& maneuver) {
  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  text_instruction += "Enter the roundabout";
  if ((maneuver.roundabout_exit_count() > 0)
      && (maneuver.roundabout_exit_count() < 11)) {
    text_instruction += " and take the ";
    text_instruction += FormOrdinalValue(maneuver.roundabout_exit_count());
    text_instruction += " exit";
  }

  text_instruction += ".";
  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormExitRoundaboutInstruction(Maneuver& maneuver) {
  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  text_instruction += "Exit the roundabout";

  if (maneuver.HasBeginStreetNames()) {
    text_instruction += " onto ";
    text_instruction += maneuver.begin_street_names().ToString();
    text_instruction += ". Continue on ";
    text_instruction += maneuver.street_names().ToString();
  } else if (maneuver.HasStreetNames()) {
    text_instruction += " onto ";
    text_instruction += maneuver.street_names().ToString();
  }

  text_instruction += ".";
  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormEnterFerryInstruction(Maneuver& maneuver) {
  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  text_instruction += "Take the ";
  if (maneuver.HasStreetNames()) {
    text_instruction += maneuver.street_names().ToString();
  }

  // TODO - handle properly with locale narrative builder
  std::string ferry_label = " Ferry";
  if (!boost::algorithm::ends_with(text_instruction, ferry_label)) {
    text_instruction += ferry_label;
  }

  text_instruction += ".";
  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormExitFerryInstruction(Maneuver& maneuver) {
  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  text_instruction += "Go ";
  text_instruction += FormCardinalDirection(
      maneuver.begin_cardinal_direction());

  if (maneuver.HasBeginStreetNames()) {
    text_instruction += " on ";
    text_instruction += maneuver.begin_street_names().ToString();
    text_instruction += ". Continue on ";
    text_instruction += maneuver.street_names().ToString();
  } else if (maneuver.HasStreetNames()) {
    text_instruction += " on ";
    text_instruction += maneuver.street_names().ToString();
  }

  text_instruction += ".";
  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormTransitConnectionStartInstruction(
    Maneuver& maneuver) {
  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  text_instruction += "Enter";
  if (!maneuver.transit_connection_stop().name.empty()) {
    text_instruction += " the ";
    text_instruction += maneuver.transit_connection_stop().name;
  }
  text_instruction += " station.";
  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormTransitConnectionTransferInstruction(
    Maneuver& maneuver) {
  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  text_instruction += "Transfer at";
  if (!maneuver.transit_connection_stop().name.empty()) {
    text_instruction += " the ";
    text_instruction += maneuver.transit_connection_stop().name;
  }
  text_instruction += " station.";
  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormTransitConnectionDestinationInstruction(
    Maneuver& maneuver) {
  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  text_instruction += "Exit";
  if (!maneuver.transit_connection_stop().name.empty()) {
    text_instruction += " the ";
    text_instruction += maneuver.transit_connection_stop().name;
  }
  text_instruction += " station.";
  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormTransitInstruction(
    Maneuver& maneuver) {
  // TODO - refactor transit instructions
  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  text_instruction += "DEPART: ";
  text_instruction += maneuver.GetFormattedTransitDepartureTime();
  text_instruction += " from ";
  text_instruction += maneuver.GetTransitStops().front().name;
  text_instruction += ". Take the ";
  text_instruction += maneuver.GetTransitName();
  if (!maneuver.transit_headsign().empty()) {
    text_instruction += " toward ";
    text_instruction += maneuver.transit_headsign();
  }
  text_instruction += ". (";
  text_instruction += std::to_string(maneuver.GetTransitStopCount());
  if (maneuver.GetTransitStopCount() > 1) {
    text_instruction += " stops";
  } else {
    text_instruction += " stop";
  }
  text_instruction += ").";
  text_instruction += " ARRIVE: ";
  text_instruction += maneuver.GetFormattedTransitArrivalTime();
  text_instruction += " at ";
  text_instruction += maneuver.GetTransitStops().back().name;
  text_instruction += ".";

  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormTransitRemainOnInstruction(
    Maneuver& maneuver) {
  // TODO - refactor transit instructions
  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  text_instruction += "DEPART: ";
  text_instruction += maneuver.GetFormattedTransitDepartureTime();
  text_instruction += " from ";
  text_instruction += maneuver.GetTransitStops().front().name;
  text_instruction += ". Remain on the ";
  text_instruction += maneuver.GetTransitName();
  if (!maneuver.transit_headsign().empty()) {
    text_instruction += " toward ";
    text_instruction += maneuver.transit_headsign();
  }
  text_instruction += ". (";
  text_instruction += std::to_string(maneuver.GetTransitStopCount());
  if (maneuver.GetTransitStopCount() > 1) {
    text_instruction += " stops";
  } else {
    text_instruction += " stop";
  }
  text_instruction += ").";
  text_instruction += " ARRIVE: ";
  text_instruction += maneuver.GetFormattedTransitArrivalTime();
  text_instruction += " at ";
  text_instruction += maneuver.GetTransitStops().back().name;
  text_instruction += ".";

  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormTransitTransferInstruction(
    Maneuver& maneuver) {
  // TODO - refactor transit instructions
  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  text_instruction += "DEPART: ";
  text_instruction += maneuver.GetFormattedTransitDepartureTime();
  text_instruction += " from ";
  text_instruction += maneuver.GetTransitStops().front().name;
  text_instruction += ". Transfer to take the ";
  text_instruction += maneuver.GetTransitName();
  if (!maneuver.transit_headsign().empty()) {
    text_instruction += " toward ";
    text_instruction += maneuver.transit_headsign();
  }
  text_instruction += ". (";
  text_instruction += std::to_string(maneuver.GetTransitStopCount());
  if (maneuver.GetTransitStopCount() > 1) {
    text_instruction += " stops";
  } else {
    text_instruction += " stop";
  }
  text_instruction += ").";
  text_instruction += " ARRIVE: ";
  text_instruction += maneuver.GetFormattedTransitArrivalTime();
  text_instruction += " at ";
  text_instruction += maneuver.GetTransitStops().back().name;
  text_instruction += ".";


  maneuver.set_instruction(std::move(text_instruction));
}

void NarrativeBuilder::FormPostTransitConnectionDestinationInstruction(
    Maneuver& maneuver) {
  std::string text_instruction;
  text_instruction.reserve(kTextInstructionInitialCapacity);
  text_instruction += "Go ";
  text_instruction += FormCardinalDirection(
      maneuver.begin_cardinal_direction());

  if (maneuver.HasBeginStreetNames()) {
    text_instruction += " on ";
    text_instruction += maneuver.begin_street_names().ToString();
    text_instruction += ". Continue on ";
    text_instruction += maneuver.street_names().ToString();
  } else if (maneuver.HasStreetNames()) {
    text_instruction += " on ";
    text_instruction += maneuver.street_names().ToString();
  }

  text_instruction += ".";
  maneuver.set_instruction(std::move(text_instruction));
}

std::string NarrativeBuilder::FormVerbalPostTransitionInstruction(
    Maneuver& maneuver, DirectionsOptions_Units units,
    bool include_street_names, uint32_t element_max_count,
    std::string delim) {
  switch (units) {
    case DirectionsOptions_Units_kMiles: {
      return FormVerbalPostTransitionMilesInstruction(
          maneuver, include_street_names, element_max_count, delim);
    }
    default: {
      return FormVerbalPostTransitionKilometersInstruction(
          maneuver, include_street_names, element_max_count, delim);
    }
  }
}

std::string NarrativeBuilder::FormVerbalPostTransitionKilometersInstruction(
    Maneuver& maneuver, bool include_street_names,
    uint32_t element_max_count, std::string delim) {
  //  TODO
  //  "Continue for one hundred meters." (.1, .2, .3, .4, .6, .7, .8, .9)
  //  "Continue for a half kilometer."
  //  "Continue for one kilometer."
  //  "Continue for 1.2 kilometers"
  //  "Continue for one and a half kilometers"
  //
  //  "Continue on <STREET_NAMES(2)> for one hundred meters." (.1, .2, .3, .4, .6, .7, .8, .9)
  //  "Continue on <STREET_NAMES(2)> for a half kilometer."
  //  "Continue on <STREET_NAMES(2)> for one kilometer."
  //  "Continue on <STREET_NAMES(2)> for 1.2 kilometers"
  //  "Continue on <STREET_NAMES(2)> for one and a half kilometers"

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction += "Continue";

  if (include_street_names && maneuver.HasStreetNames()) {
    instruction += " on ";
    instruction += maneuver.street_names().ToString(element_max_count,
                                                    delim);
  }

  instruction += " for ";
  instruction += (boost::format("%.1f") % maneuver.length()).str();
  instruction += " kilometers.";
  return instruction;
}

std::string NarrativeBuilder::FormVerbalPostTransitionMilesInstruction(
    Maneuver& maneuver, bool include_street_names,
    uint32_t element_max_count, std::string delim) {
  //  TODO
  //  "Continue for one tenth of a mile."
  //  "Continue for two tenths of a mile." (.2, .3, .4, .6, .7, .8, .9)
  //  "Continue for a half mile."
  //  "Continue for one mile."
  //  "Continue for 1.2 miles"
  //  "Continue for one and a half miles"
  //
  //  "Continue on <STREET_NAMES(2)> for one tenth of a mile."
  //  "Continue on <STREET_NAMES(2)> for two tenths of a mile." (.2, .3, .4, .6, .7, .8, .9)
  //  "Continue on <STREET_NAMES(2)> for a half mile."
  //  "Continue on <STREET_NAMES(2)> for one mile."
  //  "Continue on <STREET_NAMES(2)> for 1.2 miles"
  //  "Continue on <STREET_NAMES(2)> for one and a half miles"
  //

  std::string instruction;
  instruction.reserve(kTextInstructionInitialCapacity);
  instruction += "Continue";

  if (include_street_names && maneuver.HasStreetNames()) {
    instruction += " on ";
    instruction += maneuver.street_names().ToString(element_max_count,
                                                    delim);
  }

  instruction += " for ";
  instruction += (boost::format("%.1f")
      % maneuver.length(DirectionsOptions_Units_kMiles)).str();
  instruction += " miles.";
  return instruction;
}

std::string NarrativeBuilder::FormCardinalDirection(
    TripDirections_Maneuver_CardinalDirection cardinal_direction) {
  switch (cardinal_direction) {
    case TripDirections_Maneuver_CardinalDirection_kNorth: {
      return "north";
    }
    case TripDirections_Maneuver_CardinalDirection_kNorthEast: {
      return "northeast";
    }
    case TripDirections_Maneuver_CardinalDirection_kEast: {
      return "east";
    }
    case TripDirections_Maneuver_CardinalDirection_kSouthEast: {
      return "southeast";
    }
    case TripDirections_Maneuver_CardinalDirection_kSouth: {
      return "south";
    }
    case TripDirections_Maneuver_CardinalDirection_kSouthWest: {
      return "southwest";
    }
    case TripDirections_Maneuver_CardinalDirection_kWest: {
      return "west";
    }
    case TripDirections_Maneuver_CardinalDirection_kNorthWest: {
      return "northwest";
    }
    default: {
      throw std::runtime_error(
          "Invalid TripDirections_Maneuver_CardinalDirection in method FormCardinalDirection.");
    }
  }
}

std::string NarrativeBuilder::FormTurnTypeInstruction(
    TripDirections_Maneuver_Type type) {
  switch (type) {
    case TripDirections_Maneuver_Type_kSlightRight:
    case TripDirections_Maneuver_Type_kRight:
    case TripDirections_Maneuver_Type_kUturnRight:
    case TripDirections_Maneuver_Type_kRampRight:
    case TripDirections_Maneuver_Type_kExitRight: {
      return "right";
    }
    case TripDirections_Maneuver_Type_kSharpRight: {
      return "sharp right";
    }
    case TripDirections_Maneuver_Type_kSharpLeft: {
      return "sharp left";
    }
    case TripDirections_Maneuver_Type_kSlightLeft:
    case TripDirections_Maneuver_Type_kLeft:
    case TripDirections_Maneuver_Type_kUturnLeft:
    case TripDirections_Maneuver_Type_kRampLeft:
    case TripDirections_Maneuver_Type_kExitLeft: {
      return "left";
    }
    default: {
      throw std::runtime_error(
          "Invalid TripDirections_Maneuver_Type in method FormTurnTypeInstruction.");
    }
  }

}

std::string NarrativeBuilder::FormOrdinalValue(uint32_t value) {
  switch (value) {
    case 1: {
      return "1st";
    }
    case 2: {
      return "2nd";
    }
    case 3: {
      return "3rd";
    }
    case 4: {
      return "4th";
    }
    case 5: {
      return "5th";
    }
    case 6: {
      return "6th";
    }
    case 7: {
      return "7th";
    }
    case 8: {
      return "8th";
    }
    case 9: {
      return "9th";
    }
    case 10: {
      return "10th";
    }
    default: {
      return "undefined";
    }
  }

}

}
}

