## 第5题

验证 $y = -6\cos 2x + 8\sin 2x$ 是方程 $y'' + y' + \frac{5}{2}y = 25\cos 2x$ 的解，且满足 $y(0) = -6$，$y'(0) = 16$。

**求导：**

$y = -6\cos 2x + 8\sin 2x$

$y' = 12\sin 2x + 16\cos 2x$

$y'' = 24\cos 2x - 32\sin 2x$

**代入方程：**

$y'' + y' + \frac{5}{2}y$

$= (24\cos 2x - 32\sin 2x) + (12\sin 2x + 16\cos 2x) + \frac{5}{2}(-6\cos 2x + 8\sin 2x)$

$= (24 + 16 - 15)\cos 2x + (-32 + 12 + 20)\sin 2x$

$= 25\cos 2x$ ✓

**验证初值条件：**

$y(0) = -6\cos 0 + 8\sin 0 = -6$ ✓

$y'(0) = 12\sin 0 + 16\cos 0 = 16$ ✓

---

## 第6题

$\sqrt{1 - y^2},dx + y\sqrt{1 - x^2},dy = 0$

**分离变量：**

$$\frac{dx}{\sqrt{1-x^2}} + \frac{y,dy}{\sqrt{1-y^2}} = 0$$

注意 $\frac{y}{\sqrt{1-y^2}} = -\frac{d}{dy}\sqrt{1-y^2}$，所以：

$$\int \frac{dx}{\sqrt{1-x^2}} + \int \frac{y,dy}{\sqrt{1-y^2}} = C_1$$

$$\arcsin x - \sqrt{1 - y^2} = C$$

即 $\boxed{\arcsin x - \sqrt{1-y^2} = C}$

---

## 第7题

$y' = 2\sqrt{y}\ln x$，$y(e) = 1$

**分离变量：**

$$\frac{dy}{2\sqrt{y}} = \ln x,dx$$

$$\sqrt{y} = \int \ln x,dx = x\ln x - x + C$$

**代入初值** $y(e)=1$：

$1 = e \cdot 1 - e + C = C$，故 $C = 1$

$$\boxed{\sqrt{y} = x\ln x - x + 1}$$

---

## 第9题

$\dfrac{dy}{dx} = \dfrac{2xy}{x^2 + y^2}$

这是齐次方程。令 $y = ux$，则 $\frac{dy}{dx} = u + xu'$。

$$u + xu' = \frac{2x(ux)}{x^2 + u^2x^2} = \frac{2u}{1 + u^2}$$

$$xu' = \frac{2u}{1+u^2} - u = \frac{2u - u - u^3}{1+u^2} = \frac{u - u^3}{1+u^2} = \frac{u(1-u^2)}{1+u^2}$$

**分离变量：**

$$\frac{1+u^2}{u(1-u^2)}du = \frac{dx}{x}$$

对左边做部分分式分解。注意 $\frac{1+u^2}{u(1-u)(1+u)}$，令：

$$\frac{1+u^2}{u(1-u^2)} = \frac{A}{u} + \frac{B}{1-u} + \frac{D}{1+u}$$

解得 $A = -1$，$B = 1$，$D = 1$。

积分：

$$-\ln|u| - \ln|1-u| - \ln|1+u| + \text{（修正）}$$

重新计算：$\frac{1+u^2}{u(1-u^2)}$。令 $1+u^2 = A(1-u^2) + Bu(1+u) + Du(1-u)$。

$u=0$: $1 = A$，即 $A=1$。 $u=1$: $2 = 2B$，即 $B=1$。 $u=-1$: $2 = -2D$，即 $D=-1$。

$$\int\left(\frac{1}{u} + \frac{1}{1-u} - \frac{1}{1+u}\right)du = \ln|x| + C$$

$$\ln|u| - \ln|1-u| - \ln|1+u| = \ln|x| + C$$

$$\ln\frac{|u|}{|1-u^2|} = \ln|x| + C$$

回代 $u = y/x$：

$$\ln\frac{|y/x|}{1-y^2/x^2} = \ln|x| + C$$

$$\frac{y/x}{(x^2-y^2)/x^2} = \frac{xy}{x^2 - y^2}$$

所以：

$$\ln\frac{|xy|}{|x^2-y^2|} = \ln|x| + C$$

$$\frac{xy}{x^2 - y^2} = Cx$$

$$\boxed{\frac{y}{x^2 - y^2} = C}$$

（即 $x^2 - y^2 = Cy$）

---

## 第11题

$(y+x)dy = (y-x)dx$

$$\frac{dy}{dx} = \frac{y-x}{y+x}$$

齐次方程，令 $y = ux$：

$$u + xu' = \frac{ux - x}{ux + x} = \frac{u-1}{u+1}$$

$$xu' = \frac{u-1}{u+1} - u = \frac{u-1 - u^2 - u}{u+1} = \frac{-(1+u^2)}{u+1}$$

**分离变量：**

$$\frac{u+1}{1+u^2}du = -\frac{dx}{x}$$

$$\int\frac{u}{1+u^2}du + \int\frac{1}{1+u^2}du = -\ln|x| + C$$

$$\frac{1}{2}\ln(1+u^2) + \arctan u = -\ln|x| + C$$

回代 $u = y/x$：

$$\frac{1}{2}\ln\frac{x^2+y^2}{x^2} + \arctan\frac{y}{x} = -\ln|x| + C$$

$$\frac{1}{2}\ln(x^2+y^2) - \ln|x| + \arctan\frac{y}{x} = -\ln|x| + C$$

$$\boxed{\frac{1}{2}\ln(x^2 + y^2) + \arctan\frac{y}{x} = C}$$

---

## 第13题

$xy' - y = \sqrt{x^2 - y^2}$，$y(1) = 1/2$

$$y' = \frac{y + \sqrt{x^2 - y^2}}{x}$$

齐次方程，令 $y = ux$：

$$u + xu' = u + \sqrt{1 - u^2}$$

$$xu' = \sqrt{1 - u^2}$$

**分离变量：**

$$\frac{du}{\sqrt{1-u^2}} = \frac{dx}{x}$$

$$\arcsin u = \ln|x| + C$$

回代 $u = y/x$：

$$\arcsin\frac{y}{x} = \ln|x| + C$$

**代入初值** $y(1) = 1/2$：

$$\arcsin\frac{1}{2} = \ln 1 + C \Rightarrow C = \frac{\pi}{6}$$

$$\boxed{\arcsin\frac{y}{x} = \ln|x| + \frac{\pi}{6}}$$