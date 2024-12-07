## Concept

Allows picking a component via a drop-down menu at editor-time
and the retrieving it in C++/BP at runtime.
This allows for a more Unity-like dependency injection pattern,
where component devs don't need to provide any kind of setter
method that has to be manually called to inject the dependency.
