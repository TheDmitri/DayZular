# RangeMapper Documentation

## Overview

`RangeMapper` is a utility class for mapping numeric ranges to values using threshold logic. It's ideal for scenarios like:
- Progress state messages (0-25% = "Starting", 25-50% = "Working", etc.)
- Level/tier systems (0-10 = Bronze, 10-25 = Silver, etc.)
- Grade/score mappings (0-60 = F, 60-70 = D, etc.)

## Syntax

```c
class RangeMapper<Class TKey, Class TValue>
```

- `TKey`: The numeric type for thresholds (typically `int` or `float`)
- `TValue`: The value type to return (typically `string`, but can be any type)

## Methods

### Insert(TKey threshold, TValue value)
Adds a threshold mapping. When `GetValue(input)` is called, it returns the value for the highest threshold that is <= input.

**Returns:** `RangeMapper<TKey, TValue>` for method chaining

### SetDefault(TValue value)
Sets the value to return when no threshold matches (input is below all thresholds).

**Returns:** `RangeMapper<TKey, TValue>` for method chaining

### GetValue(TKey input)
Evaluates the input against all thresholds and returns the appropriate value.

**Returns:** `TValue` - The mapped value, or default if set, or `null` if no match

## Usage Examples

### Progress State Messages
```c
class WorkbenchInProgressComponent extends ComponentBase
{
    // Define the mapper as a static instance (created once, reused)
    protected static ref RangeMapper<int, string> s_ProgressStates;
    
    void WorkbenchInProgressComponent()
    {
        // Initialize once
        if (!s_ProgressStates)
        {
            s_ProgressStates = new RangeMapper<int, string>();
            s_ProgressStates.Insert(0, "Gathering materials...")
                           .Insert(25, "Assembling components...")
                           .Insert(50, "Finishing touches...")
                           .Insert(75, "Almost complete...")
                           .SetDefault("Done");
        }
    }
    
    void UpdateProgress()
    {
        int craftProgress = CalculateProgress(); // 0-100
        progress_state = s_ProgressStates.GetValue(craftProgress);
    }
}
```

### Inline Configuration
```c
// One-time setup with fluent syntax
RangeMapper<int, string> grades = new RangeMapper<int, string>();
grades.Insert(0, "F")
      .Insert(60, "D")
      .Insert(70, "C")
      .Insert(80, "B")
      .Insert(90, "A")
      .SetDefault("F");

// Usage
string grade = grades.GetValue(studentScore); // 85 -> "B"
```

### Float Thresholds
```c
RangeMapper<float, string> healthStatus = new RangeMapper<float, string>();
healthStatus.Insert(0.0, "Critical")
            .Insert(25.0, "Low")
            .Insert(50.0, "Moderate")
            .Insert(75.0, "Healthy")
            .Insert(90.0, "Perfect");

string status = healthStatus.GetValue(currentHealth); // 67.5 -> "Moderate"
```

### Non-String Values
```c
// Map levels to unlockable items
RangeMapper<int, ref array<string>> unlockables = new RangeMapper<int, ref array<string>>();

array<string> level1Rewards = {"BasicSword", "LeatherArmor"};
array<string> level5Rewards = {"SteelSword", "Chainmail", "Shield"};
array<string> level10Rewards = {"Excalibur", "DragonArmor"};

unlockables.Insert(1, level1Rewards)
           .Insert(5, level5Rewards)
           .Insert(10, level10Rewards);

ref array<string> rewards = unlockables.GetValue(playerLevel);
```

## How It Works

1. **Threshold Logic:** Given input value `X`, find the highest threshold `T` where `T <= X`
2. **Return Value:** Return the value associated with threshold `T`
3. **No Match:** If `X` is below all thresholds, return the default (if set) or `null`

Example with thresholds at 0, 25, 50, 75:
- Input 15 → returns value for threshold 0
- Input 30 → returns value for threshold 25
- Input 50 → returns value for threshold 50
- Input 100 → returns value for threshold 75
- Input -5 → returns default (or null)

## Performance

- Initialization: O(n²) due to sorting thresholds (typically done once)
- Lookup: O(n) where n is number of thresholds
- For high-performance scenarios with many lookups, cache the result

## Notes

- Thresholds are sorted internally; insert order doesn't matter
- Duplicate thresholds overwrite previous values
- The `SetDefault()` value is returned for inputs below all thresholds
- Generic types must support comparison operators (`>`, `>=`)
