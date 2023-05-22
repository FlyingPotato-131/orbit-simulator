# To build
```
mkdir build

cd build

cmake ..

make

./orbit_simulator
```

controls:
pl - camera focus earth
sh - camera focus ship
sp - camera space aligned
or - camera orbit (planet) aligned
tw uint - timewarp by uint
dv float float float - maneuver prograde, radial in, normal in m/s deltaV
