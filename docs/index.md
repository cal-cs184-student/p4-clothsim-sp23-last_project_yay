# CS 184: Computer Graphics and Imaging, Spring 2023

## Project 4: ClothSim
## Michael Lin, Rachel Lee
 *** 
### Part 1  
- **Take some screenshots of scene/pinned2.json from a viewing angle where you can clearly see the cloth wireframe to show the structure of your point masses and springs.**
 - ![pinned2_1](./images/part1/pinned2_1.png)
 - ![pinned2_1](./images/part1/pinned2_2.png)

 - **Show us what the wireframe looks like (1) without any shearing constraints, (2) with only shearing constraints, and (3) with all constraints.**
    - Without shearing: 
    - ![no_shearing](./images/part1/no_shearing.png)
    - Only shearing:
    - ![only_shearing](./images/part1/only_shearing.png)
     - All constraints:
    - ![all_constraints](./images/part1/all_constraints.png)

### Part 2
- **Describe the effects of changing the spring constant ks; how does the cloth behave from start to rest with a very low ks? A high ks**
    - With a high ks, the cloth becomes much tighter pulled around the stress points of where it is pinned. With a very low ks, the cloth hangs more freely and loosely.
- **What about for density?**
    - With a higher density, the cloth bounces around more rapidly at different points. With a lower density, the cloth bounces at fewer points and seems relatively more spread out.
- **What about for damping?**
    - Damping slows the rate at which the cloth falls, so with a higher damping percentage the cloth falls at a much slower pace.
- **For each of the above, observe any noticeable differences in the cloth compared to the default parameters and show us some screenshots of those interesting differences and describe when they occur.**
    - When the cloth has low ks:
    - ![low_ks](./images/part2/low_ks.png)
    - When the cloth has high ks: 
    - ![high_ks](./images/part2/high_ks.png)
    When the cloth has low ks, the top of the cloth has a bigger sag vs with a higher ks, the cloth is more taut and held tighter at the pressure points.
    - Cloth has higher density:
    - ![high_density](./images/part2/high_density.png)
    - Cloth has lower density:
    ![low_density](./images/part2/low_density.png)
    When the cloth has a higher density, it has a similar effect where the cloth does not fall as freely as with the lower density. 
    - Cloth has high damping:
    ![low_damping](./images/part2/low_damping.png)
    - Cloth has low damping:
    ![high_dampling](./images/part2/high_damping.png)
    When the cloth has low damping, we see that it falls with greater bounce and moves more freely than when the cloth has low damping. With low damping, the cloth falls much more slowly.
- **Show us a screenshot of your shaded cloth from scene/pinned4.json in its final resting state! If you choose to use different parameters than the default ones, please list them.** 
![final_resting](./images/part2/final_resting.png)
### Part 3
- **Show us screenshots of your shaded cloth from scene/sphere.json in its final resting state on the sphere using the default ks = 5000 as well as with ks = 500 and ks = 50000. Describe the differences in the results.**
- **ks = 5000:**
![final_resting](./images/part3/ks_5000.png)
- **ks = 500:**
![final_resting](./images/part3/ks_500.png)
- **ks = 50000:**
![final_resting](./images/part3/ks_50000.png)
When ks = 500, the shaded cloth's final resting state is more closely wrapped around the sphere and has a looser effect. However, as the ks factor increases, the cloth begins to land more stiffly on the sphere and in the case of ks=50000, the cloth loses its free form.
- **Show us a screenshot of your shaded cloth lying peacefully at rest on the plane. If you haven't by now, feel free to express your colorful creativity with the cloth!**
- ![lying_peacefully](./images/part3/lying_peacefully.png)