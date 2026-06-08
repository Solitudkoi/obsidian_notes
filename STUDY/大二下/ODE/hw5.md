下面逐题给出详细解答。

---

## 第26题

$$\frac{d^2x}{dt^2} + x = \cos 2t,\quad x\big|_{t=0}=0,\quad \frac{dx}{dt}\bigg|_{t=0}=-2$$

**特征方程：** $r^2+1=0$，$r=\pm i$

**齐次通解：** $x_h = C_1\cos t + C_2\sin t$

**求特解：** 由于 $\omega=2$ 不是特征根，设 $x_p = A\cos 2t$：

$$x_p''+x_p = -4A\cos 2t + A\cos 2t = -3A\cos 2t = \cos 2t \implies A=-\tfrac{1}{3}$$

**通解：** $x = C_1\cos t + C_2\sin t - \tfrac{1}{3}\cos 2t$

**代入初始条件（取 $x(0)=0$）：**

$$x(0)=C_1 - \tfrac{1}{3}=0 \implies C_1=\tfrac{1}{3}$$

$$x'(t)=-C_1\sin t + C_2\cos t + \tfrac{2}{3}\sin 2t,\quad x'(0)=C_2=-2$$

$$\boxed{x = \tfrac{1}{3}\cos t - 2\sin t - \tfrac{1}{3}\cos 2t}$$

---

## 第31题

$$2y''+5y'=\cos^2 x = \frac{1+\cos 2x}{2}$$

**特征方程：** $2r^2+5r=0$，$r=0,;r=-\tfrac{5}{2}$

**齐次通解：** $y_h = C_1 + C_2 e^{-5x/2}$

**特解分两部分：**

**(1)** 对 $f_1=\tfrac{1}{2}$：因 $r=0$ 是单根，设 $y_{p_1}=Ax$

$$5A=\tfrac{1}{2} \implies A=\tfrac{1}{10},\quad y_{p_1}=\tfrac{x}{10}$$

**(2)** 对 $f_2=\tfrac{1}{2}\cos 2x$：设 $y_{p_2}=a\cos 2x + b\sin 2x$

代入方程，比较系数：

$$\begin{cases} -8a+10b = \tfrac{1}{2} \ -10a-8b = 0 \end{cases} \implies a = -\frac{1}{41},; b = \frac{5}{164}$$

$$\boxed{y = C_1 + C_2 e^{-5x/2} + \frac{x}{10} + \frac{-4\cos 2x + 5\sin 2x}{164}}$$

---

## 第34题

$$y''+4y = x\sin 2x$$

**特征方程：** $r^2+4=0$，$r=\pm 2i$

由于 $f(x)=x\sin 2x = \text{Im}(xe^{2ix})$，而 $2i$ 是特征方程的单根（$s=1$），使用复数方法。

**解 $y''+4y=xe^{2ix}$，令 $y=e^{2ix}v$：**

$$v''+4iv'=x$$

令 $w=v'$，则 $w'+4iw=x$，用积分因子法求得特解：

$$w = \frac{x}{4i}+\frac{1}{16} = -\frac{ix}{4}+\frac{1}{16}$$

$$v = -\frac{ix^2}{8}+\frac{x}{16}$$

$$y_p = e^{2ix}!\left(-\frac{ix^2}{8}+\frac{x}{16}\right)$$

取虚部（对应 $\sin 2x$）：

$$y_p = \text{Im}(y_p) = -\frac{x^2}{8}\cos 2x + \frac{x}{16}\sin 2x$$

$$\boxed{y = C_1\cos 2x + C_2\sin 2x - \frac{x^2}{8}\cos 2x + \frac{x}{16}\sin 2x}$$

---

## 第49题

$$x^2y''-2xy'+2y = x\ln x \quad\text{（欧拉方程）}$$

**令 $x=e^t$，$D=\tfrac{d}{dt}$：** $x^2y''=D(D-1)y$，$xy'=Dy$

$$(D^2-3D+2)y = te^t$$

**特征方程：** $r^2-3r+2=0$，$r=1,2$

**齐次通解：** $y_h = C_1 e^t + C_2 e^{2t}$

**求特解：** $f(t)=te^t$，$r=1$ 是单根，令 $y=e^t v$：

$$[(D+1)^2-3(D+1)+2]v = t \implies D(D-1)v = t$$

逐步降阶：令 $u=(D-1)v$，则 $Du=t$，$u=t^2/2$

再解 $v'-v=t^2/2$，得 $v=-\tfrac{t^2}{2}-t-1$

**回代 $t=\ln x$，$e^t=x$，$e^{2t}=x^2$：**

$$\boxed{y = C_1 x + C_2 x^2 - \frac{x}{2}(\ln x)^2 - x\ln x}$$

（常数项 $-x$ 已并入 $C_1 x$）

---

## 第50题

$$x^3y'''-3x^2y''+6xy'-6y=0 \quad\text{（欧拉方程）}$$

**令 $x=e^t$，$D=\tfrac{d}{dt}$：**

$$D(D-1)(D-2)y - 3D(D-1)y + 6Dy - 6y = 0$$

展开整理：

$$(D^3-6D^2+11D-6)y = 0$$

**特征方程：** $(r-1)(r-2)(r-3)=0$，$r=1,2,3$

$$\boxed{y = C_1 x + C_2 x^2 + C_3 x^3}$$

---

## 第51题

$$y''+\frac{2}{x}y'+y=0,\quad\text{令 } y=v(x)u \text{ 化为 } u''+\lambda u=0$$

**代入 $y=vu$：**

$$vu''+(2v'+\tfrac{2v}{x})u'+(v''+\tfrac{2v'}{x}+v)u=0$$

**令 $u'$ 的系数为零：**

$$2v'+\frac{2v}{x}=0 \implies \frac{v'}{v}=-\frac{1}{x} \implies v=\frac{1}{x}$$

**此时 $u$ 的系数：** $v''=\tfrac{2}{x^3}$

$$\frac{v''}{v}+\frac{2v'}{xv}+1 = \frac{2}{x^2}-\frac{2}{x^2}+1 = 1$$

所以 $\lambda = 1$，方程变为 $u''+u=0$，解为 $u=C_1\cos x+C_2\sin x$。

$$\boxed{\lambda=1,\qquad y = \frac{C_1\cos x + C_2\sin x}{x}}$$

---

## 第52题

$$y''+y'x^{1/2}+\tfrac{1}{4}(x^{-1/2}+x-36)y = xe^{-x^{3/2}/3}$$

令 $y=v(x)u$，使 $u'$ 的系数为零：

$$2v'+vx^{1/2}=0 \implies \frac{v'}{v}=-\frac{x^{1/2}}{2} \implies v = e^{-x^{3/2}/3}$$

**计算 $u$ 的系数：**

$$v''/v = -\frac{1}{4\sqrt{x}}+\frac{x}{4}$$

$$\frac{v''}{v}+\frac{v'}{v}\cdot x^{1/2}+\frac{1}{4}(x^{-1/2}+x-36) = -\frac{1}{4\sqrt{x}}+\frac{x}{4}-\frac{x}{2}+\frac{1}{4\sqrt{x}}+\frac{x}{4}-9 = -9$$

**右端：** $xe^{-x^{3/2}/3}/v = x$

所以 $\lambda = -9$，$f(x)=x$，方程为 $u''-9u = x$。

**解此方程：** 特征根 $r=\pm 3$，特解 $u_p=-x/9$

$$\boxed{\lambda=-9,; f(x)=x,\qquad y = e^{-x^{3/2}/3}!\left(C_1 e^{3x}+C_2 e^{-3x}-\frac{x}{9}\right)}$$

---

## 第56题

$$(1-x^2)y'''-xy''+y'=0,\quad\text{已知 } y_1=x^2$$

**关键发现——方程可恰当降阶：**

注意到 $\tfrac{d}{dx}[(1-x^2)y''] = (1-x^2)y'''-2xy''$，而 $\tfrac{d}{dx}[xy']=y'+xy''$

因此原方程可写为：

$$\frac{d}{dx}\big[(1-x^2)y''+xy'\big]=0$$

即 $(1-x^2)y''+xy' = A$（常数）

**齐次情形 $(A=0)$：** $(1-x^2)p'+xp=0$，其中 $p=y'$

$$\frac{p'}{p}=\frac{-x}{1-x^2} \implies p = K\sqrt{1-x^2}$$

$$y = K\int\sqrt{1-x^2},dx = \frac{K}{2}\big(x\sqrt{1-x^2}+\arcsin x\big)+D$$

**非齐次特解：** 对 $y=x^2$，有 $(1-x^2)(2)+x(2x)=2$，故 $y=(A/2)x^2$ 对应常数 $A$。

$$\boxed{y = C_1 + C_2\big(x\sqrt{1-x^2}+\arcsin x\big) + C_3 x^2}$$

---

## 第57题

$$(1-x^2)y''+2xy'-2y=-2,\quad\text{已知 } y_1=x$$

**验证：** $y=x$ 满足齐次方程 $(1-x^2)(0)+2x(1)-2x=0$ ✓

**找特解：** 尝试 $y_p=c$（常数）：$0+0-2c=-2$，$c=1$，故 $y_p=1$。

**找齐次方程的第二个解：** 化为标准形 $y''+\tfrac{2x}{1-x^2}y'-\tfrac{2}{1-x^2}y=0$

$$P(x)=\frac{2x}{1-x^2},\quad e^{-\int P,dx} = 1-x^2$$

$$y_2 = x\int\frac{1-x^2}{x^2}dx = x!\left(-\frac{1}{x}-x\right) = -(1+x^2)$$

取 $y_2=1+x^2$。**验证：** $(1-x^2)(2)+2x(2x)-2(1+x^2)=2-2x^2+4x^2-2-2x^2=0$ ✓

$$\boxed{y = C_1 x + C_2(1+x^2)+1}$$

---

## 第58题（证明）

**设 $y_1(x),y_2(x)$ 是 $y''+p(x)y'+q(x)y=0$ 的两个线性无关解，Wronskian 为 $W(x)$。证明 $y''+p(x)y'+q(x)y=f(x)$ 的通解为：**

$$y = c_1y_1+c_2y_2+\int_{x_0}^{x}\frac{y_1(\xi)y_2(x)-y_2(\xi)y_1(x)}{W(\xi)}f(\xi),d\xi$$

**证明（常数变易法）：**

设非齐次方程有形如 $y=u_1(x)y_1(x)+u_2(x)y_2(x)$ 的解。

施加附加条件 $u_1'y_1+u_2'y_2=0$，则 $y'=u_1 y_1'+u_2 y_2'$

$$y''=u_1 y_1''+u_2 y_2''+u_1'y_1'+u_2'y_2'$$

代入原方程，利用 $y_i''+py_i'+qy_i=0$，得：

$$u_1'y_1'+u_2'y_2'=f(x)$$

联立两个方程：

$$\begin{cases} u_1'y_1+u_2'y_2=0 \ u_1'y_1'+u_2'y_2'=f(x) \end{cases}$$

由 Cramer 法则（系数行列式为 $W(x)\neq 0$）：

$$u_1'=\frac{-y_2 f}{W},\qquad u_2'=\frac{y_1 f}{W}$$

积分从 $x_0$ 到 $x$（取 $u_1(x_0)=u_2(x_0)=0$）：

$$u_1(x)=-\int_{x_0}^{x}\frac{y_2(\xi)f(\xi)}{W(\xi)}d\xi,\qquad u_2(x)=\int_{x_0}^{x}\frac{y_1(\xi)f(\xi)}{W(\xi)}d\xi$$

因此特解为：

$$y^* = \int_{x_0}^{x}\frac{y_1(\xi)y_2(x)-y_2(\xi)y_1(x)}{W(\xi)}f(\xi),d\xi$$

加上齐次通解即得所求公式。$\blacksquare$

---

## 第60题

$$y''+y=2\sec^3 x$$

**齐次通解：** $y_h=C_1\cos x+C_2\sin x$

**常数变易法：** $y_1=\cos x$，$y_2=\sin x$，$W=1$，$f(x)=2\sec^3 x$

$$u_1'=-y_2 f = -\frac{2\sin x}{\cos^3 x}=-2\tan x\sec^2 x$$

$$u_2'=y_1 f = \frac{2\cos x}{\cos^3 x}=2\sec^2 x$$

积分：

$$u_1 = -\tan^2 x,\qquad u_2 = 2\tan x$$

$$y_p = -\cos x\cdot\tan^2 x + \sin x\cdot 2\tan x = -\frac{\sin^2 x}{\cos x}+\frac{2\sin^2 x}{\cos x}=\frac{\sin^2 x}{\cos x} = \sec x - \cos x$$

其中 $-\cos x$ 并入 $C_1\cos x$：

$$\boxed{y = C_1\cos x + C_2\sin x + \sec x}$$