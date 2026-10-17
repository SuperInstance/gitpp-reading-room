# gitpp-cs crashes on kind:null — a real defect the spec change surfaced

`gitpp-cs` scores **5/7** against the seven v0.4.6-discriminating cases.
It does not disagree with the spec — it **throws**.

    Unhandled exception. System.ArgumentNullException: Value cannot be null. (Parameter 'key')
       at System.Collections.Generic.Dictionary`2.FindValue(TKey key)
       at System.Collections.Generic.CollectionExtensions.GetValueOrDefault[TKey,TValue]
       at Gitpp.Fold.Derive(IEnumerable`1 recordsList) in src/Gitpp.cs:line 215

`Gitpp.cs:215`:

```csharp
if (precedence.GetValueOrDefault(newKind, 0) > precedence.GetValueOrDefault(existingKind, 0))
```

The intent is right — an unknown kind maps to rank 0, which is exactly
what v0.4.6 now mandates. The bug is that .NET's
`Dictionary<string,int>.GetValueOrDefault(null, 0)` **throws** rather than
returning the default: a `Dictionary` rejects a null key outright,
including through the `CollectionExtensions` overload that is *supposed*
to be null-tolerant. The author wrote the correct rule and the runtime
silently disagreed.

Both failing cases are the ones with **two `kind: null` records
contesting a key** (p14, p15). A *single* null-kind record passes,
because line 215 is only reached once a key is already set.

## Why this is the best result in the whole series

- `gitpp-js`, `gitpp-c`, and `gitpp-cs` were all run **without being told
  v0.4.6 exists**. Two were accidentally right. One crashes.
- The crash is on **precisely the case the spec change was written to
  close**. The `kind: null` hole was found by two independent agents
  reading the spec, confirmed by an adjudicator at 0.840, fixed in the
  spec, and the fix immediately surfaced a real defect in a maintained
  port.
- It is a **language-specific trap**, invisible to cross-port testing that
  only checks *agreement*. Three ports agreeing on 20,043 leaf cases and
  29 fold cases would never have caught this, because no corpus case
  contained two `kind: null` records contesting a key. The existing
  29-case fold suite had no `null`-kind case at all.

That last point is the generalisable lesson, and it is the same one the
whole `gitpp-reading-room` project was built to demonstrate:

**agreement between implementations only tests the cases you thought of.**
A defect that requires a specific record shape to surface is invisible to
conformance testing by construction, and only becomes visible when a
*specification* tells you the shape is legal.

## The fix

```csharp
static int Rank(string kind) =>
    kind is null ? 0 : precedence.GetValueOrDefault(kind, 0);
```

and call `Rank(...)` at both sites. One helper, one null check, applied
consistently — the defect is that a null-tolerant *default* was confused
for a null-tolerant *dictionary*.

## Environment note (recorded so the next run is not wasted)

`dotnet build` in this sandbox fails with `NuGet.Config is not valid
XML` because `/workspace/.home/.nuget/NuGet/NuGet.Config` is a
**zero-byte file** — the known NAS write trap. It is not a project fault.
Workaround that works:

    export DOTNET_CLI_HOME=/tmp/dotnethome NUGET_PACKAGES=/tmp/nugetpkgs
    mkdir -p /tmp/dotnethome
    printf '<?xml version="1.0" encoding="utf-8"?>\n<configuration>\n  <packageSources>\n    <clear />\n  </packageSources>\n</configuration>\n' > /tmp/dotnethome/NuGet.Config
