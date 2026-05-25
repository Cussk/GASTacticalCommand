# GAS Tactical Command Framework

A focused Unreal Engine Gameplay Ability System prototype exploring **squad-level RTS command abilities** instead of the usual hero/RPG ability setup.

The project uses GAS as a tactical rules and state layer for squads: command activation, costs, cooldowns, suppression, morale recovery, fortification, retreat state, affiliation filtering, and debug visibility. High-frequency RTS simulation work such as formation movement, pathfinding, projectile simulation, and large-scale steering is intentionally kept outside GAS.

## Project Goals

This project is meant to demonstrate practical GAS proficiency in a less common gameplay context:

- Squad-level Ability System Component ownership
- RTS-focused attributes such as morale, suppression, cohesion, stamina, accuracy, defense, movement speed, and capture power
- Gameplay Effects for tactical state changes, buffs, debuffs, costs, and cooldowns
- Gameplay Tags for readable command rules and battlefield state
- Data-driven command authoring through squad definitions and order definitions
- Clean separation between command metadata, squad grants, GAS execution, targeting/query systems, and debug UI
- Debug-first workflow for inspecting active effects, tags, attributes, order results, cooldowns, and relationships

The long-term direction could become a reusable tactical GAS command framework or plugin, but V1 is intentionally a compact vertical slice.

## Core Design Thesis

GAS should answer:

> Can this squad perform this command, what does it cost, what state does it apply, and what gameplay rules change while it is active?

Other RTS systems should answer:

> Where does the squad move, how do units steer, what does the formation look like, and how are many entities simulated efficiently?

This project uses GAS for **rules, state, costs, cooldowns, tags, effects, and tactical command behavior**. It does not try to make GAS responsible for full RTS simulation.

## Current Implemented Features

### Squad Framework

- Squad-level actor owns the Ability System Component
- Squad AttributeSet includes tactical attributes such as:
  - Morale
  - Suppression
  - Cohesion
  - Stamina
  - Accuracy
  - Defense
  - Movement Speed
  - Capture Power
- Squad definitions grant startup abilities through data
- Order definitions are metadata-only and do not duplicate GAS rules
- Three squad types are currently represented by Blueprint variants:
  - Rifle Squad
  - Engineer Squad
  - Heavy Weapons Squad

Later, squad BPs can be replaced by one base squad actor populated entirely from data assets, including meshes, animation blueprint, default attributes, role tags, ability grants, UI metadata, and visual setup.

### Order and Ability Flow

The command flow is intentionally GAS-aligned:

```text
OrderDefinition
    UI/catalog metadata
    Targeting metadata
    Order tag

SquadDefinition
    Grants Gameplay Abilities
    Uses input/order tags as the bridge

OrderSubsystem / Order Component
    Validates source and target shape/range
    Finds granted ability by order tag
    Asks GAS to activate the ability

Gameplay Ability
    Owns activation rules
    Owns required/blocked tags
    Owns cost/cooldown
    Owns command behavior

Gameplay Effects
    Own stat changes
    Own granted state tags
    Own costs and cooldown tags
```

This avoids fighting GAS or duplicating GAS rules in the order layer.

### Implemented Commands

#### Rally

A self-targeted support command that recovers squad state.

Demonstrates:

- GAS cost
- GAS cooldown
- Self-applied recovery effects
- Morale/suppression/cohesion changes
- Metadata-only order definition

#### Suppressive Fire

An area-targeted command that applies pressure to enemy squads.

Demonstrates:

- Area targeting payload
- Query subsystem usage
- Relationship-based target filtering
- Source state effects
- Enemy target effects
- Suppression, morale, and cohesion pressure
- GAS cost/cooldown/state tags

#### Fortify Position

A self-targeted defensive posture command.

Demonstrates:

- Stationary requirement through GAS tags
- Temporary defensive/accuracy/cohesion buffs
- Fortified state tag
- Cost/cooldown through Gameplay Effects
- Data-authored posture state

#### Tactical Retreat

A self-targeted retreat posture command.

Demonstrates:

- Retreating state tag
- Movement speed and defensive tradeoffs
- Offensive/capture tradeoffs
- Cost/cooldown through Gameplay Effects
- Suppression recovery / cohesion tradeoff
- Data-authored removal of conflicting posture effects using built-in GAS effect removal

### Affiliation and Relationship Filtering

The project includes an opt-in affiliation component so tactical actors can participate in relationship logic without hardcoding relationships onto squad actors.

Current relationship categories:

- Own
- Friendly
- Neutral
- Enemy

Current shape:

```text
TCFAffiliationComponent
    OwnerId
    TeamId
    FactionTag

TCFRelationshipSubsystem
    Resolves Own/Friendly/Neutral/Enemy

TCFSquadQuerySubsystem
    Finds squads spatially
    Filters by relationship
```

This is designed to support future use cases such as:

- Player-owned squads
- Allied players
- Enemy players
- Neutral entities
- AI factions
- AI groups
- Buildings
- Commanders
- Capture objectives

Suppressive Fire currently uses this layer to affect enemy squads only.

### Debug Module

The project has a separate debug module rather than putting debug formatting/rendering in gameplay classes.

Implemented debug features:

- Native C++ on-screen debug HUD
- Selected squad display
- Squad attributes
- Owned Gameplay Tags
- Granted abilities and dynamic spec source tags
- Active Gameplay Effects and durations
- Last submitted order/result
- Affiliation data
- Nearby squad relationship display
- Live updates when switching selected squads
- Live refresh for attributes, tags, cooldowns, and active effects

Debug ownership direction:

```text
GASTacticalCommandDebug -> depends on -> GASTacticalCommand
GASTacticalCommand      -> does not depend on -> GASTacticalCommandDebug
```

Gameplay classes expose state. The debug module formats and displays it.

## Architecture Notes

### Preferred Style

The project favors:

- Components and subsystems over deep inheritance
- Data-oriented structs and definitions where practical
- Shallow class hierarchies
- GAS-owned rules instead of duplicated gameplay checks
- Event/timer/state-driven behavior over unnecessary ticking
- Debug-specific systems instead of debug logic inside gameplay classes
- Explicit ownership and data flow

### Pattern Parallels

The project uses traditional pattern ideas where they naturally fit Unreal architecture, without forcing pattern-heavy OOP:

- **Command pattern parallel**: player order requests represent command intent
- **Adapter**: the order subsystem adapts RTS order intent into GAS activation
- **Strategy**: individual Gameplay Abilities own command behavior
- **Policy service**: relationship subsystem resolves tactical affiliation rules
- **Observer/View Model**: debug subsystem observes selected squad/order state and builds display snapshots
- **Component pattern**: affiliation, selection, movement, and debug bridge behavior are componentized

Patterns are used only where they improve ownership, data flow, debugging, or extensibility.

## Key Systems

### Order Definitions

Order definitions should stay metadata-focused:

- Display name
- Order tag
- UI/catalog metadata
- Targeting shape/type/range/radius
- Debug visibility

They should not own:

- Ability class
- Cost
- Cooldown
- Blocked tags
- Required tags
- Gameplay effects
- Execution behavior

### Squad Definitions

Squad definitions grant the actual order abilities:

```text
AbilityClass: GA_Order_Rally
InputTag: Order.Type.Rally
AbilityLevel: 1
```

This keeps execution availability authored in one place.

### Gameplay Abilities

Gameplay Abilities own:

- Activation requirements
- Blocked tags
- Cost GameplayEffect
- Cooldown GameplayEffect
- Execution behavior
- Self/target effect application

### Gameplay Effects

Gameplay Effects own:

- Attribute changes
- State tags
- Costs
- Cooldown tags
- Duration/infinite/instant policy
- Removal of conflicting active effects where appropriate

## Current V1 Status

Completed or in progress:

- [x] Squad-level ASC architecture
- [x] Tactical AttributeSet
- [x] Native Gameplay Tags for squad/order state
- [x] Order definitions as metadata
- [x] Squad definitions granting abilities
- [x] Rally
- [x] Suppressive Fire
- [x] Fortify Position
- [x] Tactical Retreat
- [x] Relationship-aware query filtering
- [x] Affiliation component
- [x] Debug module and native debug HUD
- [x] Rifle, Engineer, and Heavy Weapons squad variants
- [ ] Minimal squad movement
- [ ] Capture points
- [ ] Cover zones
- [ ] Basic enemy behavior
- [ ] Gameplay Cue / visual feedback pass
- [ ] Small playable V1 loop

## Planned Near-Term Phases

### Phase 12: Minimal Squad Movement Foundation

Add selected squad click-to-move at the squad actor level.

Planned scope:

- Squad movement component
- Player movement command component
- Movement uses MovementSpeed attribute
- Moving/Stationary GAS state effects
- Fortify blocked while moving via required tags
- Movement starts remove Fortify/Suppressing effects through built-in GE removal

Out of scope:

- Full formation movement
- Individual soldier steering
- Move attack
- Multiplayer prediction
- Advanced pathfinding

### Phase 13: Capture Points

Add the first objective loop after minimal movement exists.

Planned scope:

- Capture point actor
- Occupying squad tracking
- Capture progress
- Contested state
- CapturePower attribute usage
- Affiliation ownership
- Debug visibility

### Later V1 Work

Potential follow-up work:

- Cover zone actors
- Capture point UI/debug
- Enemy squad test behavior
- Basic win/loss condition
- Gameplay Cues for ability activation and state feedback
- Cleaner input/UI command palette
- Data-driven squad spawning from buildings

## Non-Goals for V1

V1 intentionally avoids:

- Full economy
- Worker harvesting
- Base construction
- Tech trees
- Large-scale AI
- Complex multiplayer
- Dozens of unit types
- Full projectile ballistics
- Individual soldier ASC usage
- Full RTS campaign structure

These may be useful later, but they are not needed to prove the GAS command architecture.

## Development Notes

### Adding a New Order Ability

Recommended flow:

1. Add or confirm native gameplay tags.
2. Create an order definition with metadata and targeting only.
3. Create a Gameplay Ability class if custom behavior is needed.
4. Create Gameplay Effects for cost, cooldown, state, and modifiers.
5. Grant the ability from the squad definition using the order/input tag.
6. Test activation, cost, cooldown, state tags, and debug visibility.
7. Avoid adding command-specific rules to the order subsystem unless they are truly generic order validation.

### Adding a New Squad Type

Recommended flow:

1. Create or update a squad definition.
2. Set role tag and base attributes.
3. Grant appropriate order abilities.
4. Create a temporary BP variant if needed.
5. Set affiliation values in the test map.
6. Verify debug HUD displays role, attributes, abilities, effects, and relationships.

Later, replace per-squad BP variants with one base actor populated from data.

### Adding a New Relationship-Aware Query

Recommended flow:

1. Use `TCFSquadQuerySubsystem` for spatial squad discovery.
2. Use `TCFRelationshipSubsystem` for relationship resolution.
3. Pass an explicit relationship filter such as enemy-only or own-and-friendly.
4. Keep relationship rules out of abilities.

## V1 Status

V1 is complete as a tactical command framework prototype.

Implemented:

- Data-driven squad definitions
- Rifle, Engineer, and Heavy Weapons squad types
- Squad selection
- Minimal squad-level movement
- GAS-routed order activation
- Rally
- Suppressive Fire
- Fortify Position
- Tactical Retreat
- GAS-owned cost, cooldown, state tags, and stat effects
- Affiliation component for owner/team/faction identity
- Relationship subsystem for Own/Friendly/Neutral/Enemy resolution
- Relationship-filtered tactical queries
- Affiliation-based capture points
- Native debug HUD
- Squad, ability, effect, relationship, and capture point debug display

V1 intentionally does not include production buildings, economy, advanced RTS pathfinding, enemy AI, match scoring, fog of war, or multiplayer replication polish. Those belong to V2 planning.
