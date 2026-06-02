# GAS Tactical Command Framework

A focused Unreal Engine 5 C++ / Gameplay Ability System project exploring **GAS integration in an RTS-style framework**.

The project started as a squad-level tactical command prototype and is now expanding into a broader RTS MVP direction. GAS is used for gameplay rules, activation checks, costs, cooldowns, state tags, Gameplay Effects, unlock rules, and tactical command behavior. RTS-specific systems such as selection, movement, command routing, production queues, building placement, spawning, and world interaction stay in dedicated systems.

## Current Stage

**V1 is complete** as a tactical command framework prototype.

**V2 is in progress** as the RTS MVP expansion. Current V2 direction includes:

- RTS camera and selection controls
- Hover context and custom cursor feedback
- Command routing for right-click and UI-driven actions
- Basic squad attack behavior
- Resource and building interaction
- Gathering and production foundations
- Building, squad production, and research rules
- Continued GAS integration without forcing all RTS logic into GAS

## Core Thesis

GAS is the **rules and state layer** for tactical RTS commands.

GAS should answer:

> Can this unit, squad, building, or player command source perform this action right now? What does it cost? What tags, cooldowns, requirements, and gameplay state changes apply?

Dedicated RTS systems should answer:

> Where does the actor move? How does selection work? How is a queue processed? Where does an actor spawn? How is placement preview validated? How are many entities simulated efficiently?

This keeps GAS focused on the work it is good at while allowing RTS systems to stay simple, direct, and performance-aware.

## GAS Integration Rules

### 1. GAS owns gameplay rule validation

Gameplay Abilities should own or coordinate:

- Activation requirements
- Blocked and required tags
- Cost Gameplay Effects
- Cooldown Gameplay Effects
- Temporary state tags
- Buffs and debuffs
- Ability-specific execution rules
- Research, building level, or unlock requirements when applicable

### 2. GAS should not own high-frequency RTS simulation

GAS should not be responsible for:

- Per-frame movement
- Formation steering
- Large-scale pathing
- Continuous projectile simulation
- Selection box logic
- Cursor traces every frame
- Production queue ticking
- Raw actor spawning as the main implementation detail

Those systems should live in RTS-focused components, subsystems, or actors.

### 3. Production, construction, and research are GAS-backed, not GAS-owned

Production, construction, and research actions should be exposed through UI and validated through GAS, but existing RTS systems should still execute the actual work.

GAS handles:

- Can this action start?
- Are the required resources available?
- Is the building in a valid state?
- Is the required research complete?
- Is the action blocked by tags?
- Should a cost or cooldown be committed?
- Should an unlock, state tag, or Gameplay Effect be applied?

Production/build/research systems handle:

- Queue slots
- Timers
- Placement previews
- Construction site or building creation
- Squad spawning
- Rally point output
- Completion events

### 4. UI actions should map to data and abilities

UI buttons should not hardcode gameplay rules.

A selected squad, building, or player command source should expose available actions from granted abilities, data definitions, tags, and current state.

Example:

```text
Selected Barracks UI
    -> Player clicks Train Rifle Squad
        -> Barracks ASC activates a production ability
            -> Ability validates cost, tags, research, building state, and queue availability
            -> Ability commits cost/cooldown
            -> Production component starts queue/timer
                -> Existing build/spawn component creates squad at rally point
```

### 5. Debug stays out of gameplay classes

Gameplay classes expose state. Debug systems format and display it.

The debug module depends on gameplay code, but gameplay code does not depend on the debug module.

```text
GASTacticalCommandDebug -> depends on -> GASTacticalCommand
GASTacticalCommand      -> does not depend on -> GASTacticalCommandDebug
```

## Implemented V1 Features

### Squad Framework

- Squad-level actor owns the Ability System Component
- Tactical AttributeSet includes:
  - Health
  - Morale
  - Suppression
  - Cohesion
  - Stamina
  - Accuracy
  - Defense
  - Movement Speed
  - Capture Power
- Squad definitions grant startup abilities through data
- Order definitions stay focused on UI/catalog/targeting metadata
- Rifle, Engineer, and Heavy Weapons squad variants
- Squad selection and minimal squad-level movement
- Squad integrity/life-state handling for death and future cohesion/morale presentation

### GAS-Routed Tactical Commands

Implemented command abilities:

- Rally
- Suppressive Fire
- Fortify Position
- Tactical Retreat

These demonstrate:

- GAS cost and cooldown usage
- Gameplay Tags for state and command blocking
- Gameplay Effects for buffs, debuffs, costs, cooldowns, and tactical state
- Area and self-targeted command flow
- Relationship-aware target filtering
- Metadata-driven order definitions without duplicating GAS rules

### Basic Combat

- Basic attack behavior
- Ranged hit trace support
- Melee sweep trace support
- Temporary destroy-on-death behavior
- Direction toward future squad integrity, cohesion, morale, and visual member loss

### Affiliation and Relationship Filtering

The project includes an opt-in affiliation component so squads, buildings, objectives, and future AI groups can participate in relationship rules.

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

This supports future player relationships, AI factions, neutral entities, commanders, buildings, capture objectives, and enemy targeting rules.

### Capture Points

V1 includes affiliation-based capture points and debug visibility for objective state.

### Debug Module

The project has a separate debug module with a native C++ debug HUD.

Implemented debug visibility includes:

- Selected squad state
- Attributes
- Owned Gameplay Tags
- Granted abilities and source tags
- Active Gameplay Effects and durations
- Last submitted order/result
- Affiliation data
- Nearby squad relationship display
- Capture point debug display
- Live refresh while switching selected squads

## V2 RTS MVP Direction

V2 expands the project from a tactical GAS command slice into a more complete RTS MVP foundation.

Current and planned V2 areas:

- RTS camera panning, edge movement, and zoom
- Click and drag selection
- Hover context
- Custom RTS cursors
- Contextual right-click routing
- Basic attacks as abilities
- Resource nodes
- Worker/resource gathering
- Building interaction
- Production buildings
- Squad spawning through building systems
- Building construction
- Research building and tech unlock direction
- Squad levels
- Match scoring and win conditions
- Basic command UI

## Production, Construction, and Research Direction

Production, construction, and research should use generic GAS-backed action patterns.

Recommended generic ability types:

```text
GA_ProduceSquad
GA_ConstructBuilding
GA_ResearchTechnology
```

The ability validates and commits the action. The data definition describes the output.

Example production definition fields:

```text
Display Name
Icon
Description
Output Actor / Squad Class
Production Time
Resource Cost
Cost Gameplay Effect
Cooldown Gameplay Effect
Required Gameplay Tags
Blocked Gameplay Tags
Granted Completion Tags
Required Building Level
Required Research Tags
Required Faction / Relationship Rules
Queue Category
Rally Point Behavior
UI Category
Cursor / Preview Metadata
```

### Building-Specific Production

Production buildings own or receive the actions they can perform.

```text
Barracks ASC
    -> Produce Rifle Squad
    -> Produce Engineer Squad
    -> Produce Heavy Weapons Squad

Factory ASC
    -> Produce Vehicle Squad
    -> Produce Advanced Armor Squad

Research Lab ASC
    -> Research Improved Logistics
    -> Research Advanced Targeting
```

### Player-Level Construction

Global building placement should usually belong to the player, commander, or RTS command source.

```text
PlayerState / Commander ASC / RTS Command Source ASC
    -> Construct Barracks
    -> Construct Refinery
    -> Construct Research Lab
```

Placement preview and terrain validation stay outside GAS. Confirmed construction activation goes through GAS.

## Command and Ability Flow

The command flow is intentionally GAS-aligned:

```text
OrderDefinition
    UI/catalog metadata
    Targeting metadata
    Order tag

SquadDefinition / BuildingDefinition / ProductionDefinition
    Grants or describes available actions
    Uses tags as the bridge between UI intent and GAS activation

Command Router / Order Component / UI
    Resolves selected source
    Resolves target or action payload
    Requests ability activation

Gameplay Ability
    Owns activation rules
    Owns required/blocked tags
    Owns cost/cooldown
    Owns command behavior or start request

Gameplay Effects
    Own stat changes
    Own granted state tags
    Own costs and cooldown tags

RTS Component / Subsystem
    Handles movement, queueing, spawning, placement, or world behavior
```

## Architecture Style

The project favors:

- Components and subsystems over deep inheritance
- Data-driven definitions where practical
- Shallow base classes
- GAS-owned rules instead of duplicated gameplay checks
- Event/timer/state-driven behavior over unnecessary ticking
- Debug-specific systems instead of debug logic inside gameplay classes
- Explicit ownership and data flow
- Performance-aware Unreal C++ implementation

## Pattern Parallels

The project uses traditional pattern ideas where they naturally fit Unreal architecture:

- **Command pattern parallel**: player order requests represent command intent
- **Adapter**: command routing adapts RTS intent into GAS activation
- **Strategy**: Gameplay Abilities own command behavior
- **Policy service**: relationship subsystem resolves tactical affiliation rules
- **Observer/View Model**: debug systems observe selected state and build display snapshots
- **Component pattern**: affiliation, selection, movement, production, and integrity behavior are componentized

Patterns are only useful here when they improve ownership, data flow, debugging, or extensibility.

## Development Notes

### Adding a New Order Ability

1. Add or confirm native gameplay tags.
2. Create an order definition with metadata and targeting only.
3. Create a Gameplay Ability class if custom behavior is needed.
4. Create Gameplay Effects for cost, cooldown, state, and modifiers.
5. Grant the ability from the squad definition using the order/input tag.
6. Test activation, cost, cooldown, state tags, and debug visibility.
7. Keep command-specific rules out of the order subsystem unless they are truly generic validation rules.

### Adding a New Squad Type

1. Create or update a squad definition.
2. Set role tag and base attributes.
3. Grant appropriate order abilities.
4. Create a temporary Blueprint variant if needed.
5. Set affiliation values in the test map.
6. Verify debug HUD display for role, attributes, abilities, effects, and relationships.

Long-term, squad variants can move toward one base squad actor populated by data assets.

## To Be Implemented

### Adding a New Production Action

1. Create or update the production definition.
2. Define cost, cooldown, required tags, blocked tags, building requirements, and output class.
3. Grant or expose the action from the relevant building, player, or command source.
4. Let GAS validate and commit the action.
5. Let the production/build component own queueing, timing, and spawning.
6. Add debug visibility for activation failure, cost failure, queue state, and completion.

### Adding a New Research Unlock

1. Create or update the research definition.
2. Define cost, duration, prerequisites, required tags, and completion tags.
3. Activate the research action through a research building or command source.
4. Commit the cost/cooldown through GAS.
5. Complete through the research/production component.
6. Grant unlock tags, ability sets, production actions, building actions, squad abilities, or era/tech milestones.
