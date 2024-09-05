Spaces:

- MeshSpace (often omitted): relative to a certain mesh in a model. E.g. mesh has it's own front vector, because it's rotated 45 degrees to the left, so it's front is 45 degs to the left from model's front. No need in it when all transforms are applied to meshes (In Blender: Shift + A -> Apply transforms. That recalculates mesh space triangle coords to model space. If mesh's origin it at (2, 0, 0) and it's vertex had coords (1, 2, 3), then in MS it will have coords (3, 2, 3))
- ModelSpace (MS): relative to model's origin
- WorldSpace (WS): relative to world's origin (0, 0, 0)
- ViewSpace (VS): relative to camera's position and rotation. Vertex with coords (0, 0, 1) is 1 unit in front of the camera (Or 1 at the back. Depends on the chosen coordinate system (Using projection matrix))

Also

- TangentSpace: relative to a triangle's surface

---

Space Transformation Matrices naming on different sites

model = MS -> WS

world = WS -> VS

etc..

Inversed matrices do the opposite thing, e.g.

modelInv = WS -> MS


---

**View Matrix** is essentially an **inversed Model Matrix**. If we want to place a camera on a certain position and have a certain rotation, we can construct a Model Matrix with the given data and inverse it to get the View Matrix.

---

"A rotation matrix is just a transform that expresses the basis vectors of the input space in a different orientation."

https://paroj.github.io/gltut/Positioning/Tut06%20Rotation.html#:~:text=A%20rotation%20matrix%20is%20just,basis%20vectors%20will%20not%20change.

We can construct a rotation matrix using basis vectors (orths) the following way:

$$
\begin{pmatrix}
  R_x & U_x & F_x \\
  R_y & U_y & F_y \\
  R_z & U_z & F_z
\end{pmatrix}
$$

The rotation matrix can be transposed to rotate in the opposite direction.

Notice that viewMatrix is an inversed rotation matrix because we want to rotate the world in the opposite direction from where we want to move

$$
\begin{pmatrix}
  R_x & R_y & R_z \\
  U_x & U_y & U_z \\
  F_x & F_y & F_z
\end{pmatrix}
$$

$$
\begin{pmatrix}
  R_x & R_y & R_z & 0\\
  U_x & U_y & U_z & 0\\
  F_x & F_y & F_z & 0\\
  P_x & P_y & P_z & 1
\end{pmatrix}
$$

Also, viewMatrix can be composed from Rotation and Translation matrices



---

Space transform matrices should be read right to left

$$
positionCS = projection * view * model * positionMS
$$

Means:

$$
model -> world -> view -> clip
$$

---

When we want to go from ClipSpace to ModelSpace, the position should still be on the right, however the order of transformation matrices is inversed:

$$
positionMS = modelInv * viewInv * projectionInv * positionCS
$$

Because firstly we go from ClipSpace to ViewSpace. Remember, we should read multiplication from the right to the left:

$$
clip -> view -> world -> model
$$

---

$$
projection * view * model * position
$$

Is equal to

$$
(projection * (view * (model * position)))
$$

Because matrix multiplication is associative, i.e.

$$
(A * B) * C = A * (B * C)
$$

The only thing that matters is the order of elements

---

In general case, **view * model** is not equal to  **transpose(model * view)**, because

$$
transpose(A * B) = transpose(A) * transpose(B)
$$

$$
transpose(view * model) = transpose(view) * transpose(model)\\
transpose(view * model) = viewInv * modelInv\\
$$

As you may notice, the order of inverse transformation is wrong as we need to go from ViewSpace to WorldSpace at first

positionMS **is not equal to** viewInv * modelInv * positionCS

---

However, the following is correct

$$
matrix * position = position * transpose(matrix)
$$

It comes from how we conjugate and multiply matrices and row/column vectors

https://medium.com/geekculture/right-and-left-matrix-multiplication-d21947f195d8

![row vector multiplication](Assets/row_vector_multiplication.webp)

![column vector multiplication](Assets/column_vector_multiplication.webp)

<!-- ??? Not. Only for transpose

Let's substitute MVP

$$
matrix * position = position * transpose(matrix)\\
projection * view * model * position = position * projectionInv * viewInv * modelInv
$$ -->

---

The same way of groupping applies to the inversion

$$
inverse(A * B) = inverse(A) * inverse(B)
$$

---

More examples of groupping

$$
(2 * A * B^T + C)^T = 2 * B * A^T + C^T
$$

The sum **can't** be decomposed that way with inversion. However, there are more complex lemmas and formulas about representing inverse of the sum of elements with member matrices and their inversion

---

In hlsl there are **row_major** and **col_major** matrices and the latter are default when it comes to **input layouts** and **constant buffers**.

But when you create matrices in code, its **constructor** is always **row_major**

$$
float3x3(a11, a12, a13, a21, a22, a23, a31, a32, a33)
$$

for

$$
\begin{pmatrix}
  a11 & a12 & a13 \\
  a21 & a22 & a23 \\
  a31 & a32 & a33
\end{pmatrix}
$$

and

$$
float3x3(A, B, C)
$$

for

$$
\begin{pmatrix}
  A_x & A_y & A_z \\
  B_x & B_y & B_z \\
  C_x & B_y & C_z
\end{pmatrix}
$$

---

Transformation matrices apply in the following order:

$$
scale -> rotation -> translation
$$

---

**TangentSpace** is relative to the triangle and if we want to work with it in a certain space, we need to move it to that space at first. We can do that both ways: transforming basis vectors and creating matrice out of them or creating matrice at first and then transforming it.

$$
normalWS = model * mesh * tbn * normalTS
$$

That way, we get a normal in TangentSpace, move it to ModelSpace and then to WorldSpace

"Here we first transform all the TBN vectors to the coordinate system we'd like to work in, which in this case is world-space as we multiply them with the model matrix. Then we create the actual TBN matrix by directly supplying mat3's constructor with the relevant column vectors."

https://learnopengl.com/Advanced-Lighting/Normal-Mapping

```cpp
// Remember, no row_major
glm::vec3 rightMS(1, 0, 0);
glm::vec3 upMS(0, 1, 0);
glm::vec3 frontMS(0, 0, 1);

glm::mat3 transform = glm::eulerAngleY(glm::radians(32.77f));
glm::vec3 rightWS = transform * rightMS;
glm::vec3 upWS = transform * upMS;
glm::vec3 frontWS = transform * frontMS;

glm::mat3 tbnMS(
  rightMS,
  upMS,
  frontMS
);

glm::mat3 tbnWS0(
  rightWS,
  upWS,
  frontWS
);
glm::mat3 tbnWS1(
  rightMS,
  upMS,
  frontMS
);
tbnWS1 = transform * tbnWS1;

std::cout << "TBN MS:\n" << tbnMS << '\n';
std::cout << "TBN 0-hypothesis:\n" << tbnWS0 << '\n';
std::cout << "TBN rotate hypothesis:\n" << tbnWS1 << '\n';

// Here tbnWS0 == tbnWS1
```

If we have a mesh in **MeshSpace** that is a part of a model in **ModelSpace**, and we want to transform it's micronormal to the **WorldSpace**, then we need to cast TBN components to ModelSpace, then to WorldSpace.

---

TBN matrix is constructed the same way as we construct a rotation matrix:

$$
\begin{pmatrix}
  T_x & B_x & N_x\\
  T_y & B_y & N_y\\
  T_z & B_z & N_z
\end{pmatrix}
$$

---

Rotation matrix is a model matrix. We can think of its construction as S * R * T with S and T = identity

---

When generating IBL, we are not moving coordinates into the basis. We are drawing in the basis (in CS), abstracting ourselves from cube faces, so we need to get coordinates from basis

$$
  coordWS = inverse(basis) * coordWS
$$
