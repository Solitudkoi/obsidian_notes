# 习题一（pp.58-59）

## 第49题

$xy'' + y' = 4x$

**不显含 $y$，令 $p = y'$：**

$$xp' + p = 4x$$

$$p' + \frac{1}{x}p = 4$$

一阶线性方程，积分因子 $\mu = e^{\int \frac{1}{x}dx} = x$（在 $x>0$ 或 $x<0$ 上）：

$$d(xp) = 4x,dx$$

$$xp = 2x^2 + C_1$$

$$p = y' = 2x + \frac{C_1}{x}$$

再积分：

$$\boxed{y = x^2 + C_1\ln|x| + C_2}$$

---

## 第51题

$yy'' - (y')^2 = y^4$，$y(0)=1$，$y'(0)=0$

**不显含 $x$，令 $p = y'$，$y'' = p\dfrac{dp}{dy}$：**

$$$$

这是关于 $p$ 的伯努利方程。令 $z = p^2$，则 $\frac{dz}{dy} = 2p\frac{dp}{dy}$：

$$\frac{y}{2}\frac{dz}{dy} - z = y^4$$

$$\frac{dz}{dy} - \frac{2}{y}z = 2y^3$$

积分因子 $\mu = e^{-\int \frac{2}{y}dy} = \frac{1}{y^2}$：

$$d!\left(\frac{z}{y^2}\right) = 2y,dy$$

$$\frac{z}{y^2} = y^2 + C_1$$

$$p^2 = y^4 + C_1 y^2$$

代入 $y(0)=1, y'(0)=0$：$0 = 1 + C_1$，故 $C_1 = -1$。

$$p^2 = y^4 - y^2 = y^2(y^2-1)$$

$$(y')^2 = y^2(y^2 - 1)$$

由初值 $y(0)=1$ 且 $y'(0)=0$，在 $x=0$ 附近 $y \approx 1$。分离变量：

$$\frac{dy}{y\sqrt{y^2-1}} = \pm dx$$

注意 $\int \frac{dy}{y\sqrt{y^2-1}} = \text{arcsec}|y| = \arccos\frac{1}{y}$（当 $y \geq 1$）。

$$\arccos\frac{1}{y} = \pm x + C_2$$

代入 $y(0)=1$：$\arccos 1 = C_2$，故 $C_2 = 0$。

$$\frac{1}{y} = \cos x$$

$$\boxed{y = \sec x}$$

验证：$y' = \sec x\tan x$，$y(0)=1$ ✓，$y'(0)=0$ ✓。

---

## 第53题

$2y'' = 3y^2$，$y(-2)=1$，$y'(-2)=1$

**不显含 $x$，令 $p = y'$，$y'' = p\dfrac{dp}{dy}$：**

$$2p\frac{dp}{dy} = 3y^2$$

分离变量：

$$2p,dp = 3y^2,dy$$

$$p^2 = y^3 + C_1$$

代入初值：$y(-2)=1, y'(-2)=1$，所以 $1 = 1 + C_1$，$C_1 = 0$。

$$p^2 = y^3 \implies y' = \pm y^{3/2}$$

由 $y(-2)=1>0$，$y'(-2)=1>0$，取正号：

$$y' = y^{3/2}$$

分离变量：

$$y^{-3/2}dy = dx$$

$$-2y^{-1/2} = x + C_2$$

代入 $y(-2)=1$：$-2 = -2 + C_2$，故 $C_2 = 0$。

$$-2y^{-1/2} = x \implies y^{-1/2} = -\frac{x}{2}$$

$$\boxed{y = \frac{4}{x^2}}$$

（定义域 $x < 0$，与初值点 $x=-2$ 一致。）

---

## 第54题

$y''(1-y) + 2(y')^2 = 0$

**不显含 $x$，令 $p = y'$，$y'' = p\dfrac{dp}{dy}$：**

$$p\frac{dp}{dy}(1-y) + 2p^2 = 0$$

若 $p \neq 0$，两边除以 $p$：

$$\frac{dp}{dy}(1-y) + 2p = 0$$

$$\frac{dp}{dy} = \frac{-2p}{1-y} = \frac{2p}{y-1}$$

分离变量：

$$\frac{dp}{p} = \frac{2,dy}{y-1}$$

$$\ln|p| = 2\ln|y-1| + C$$

$$p = C_1(y-1)^2$$

即 $\dfrac{dy}{dx} = C_1(y-1)^2$，再分离变量：

$$\frac{dy}{(y-1)^2} = C_1,dx$$

$$-\frac{1}{y-1} = C_1 x + C_2$$

$$\boxed{\frac{1}{y-1} = -(C_1 x + C_2)}$$

即 $y = 1 - \dfrac{1}{C_1 x + C_2}$。另外 $p=0$ 即 $y = C$（常数）也是解。

---

## 第56题

$xy'' = y'\ln\dfrac{y'}{x}$

**不显含 $y$，令 $p = y'$：**

$$xp' = p\ln\frac{p}{x}$$

令 $v = \dfrac{p}{x}$，则 $p = vx$，$p' = v + xv'$：

$$x(v + xv') = vx\ln v$$

$$v + xv' = v\ln v$$

$$xv' = v\ln v - v = v(\ln v - 1)$$

分离变量：

$$\frac{dv}{v(\ln v - 1)} = \frac{dx}{x}$$

令 $t = \ln v - 1$，则 $dt = \frac{dv}{v}$：

$$\frac{dt}{t} = \frac{dx}{x}$$

$$\ln|t| = \ln|x| + C$$

$$t = C_1 x$$

回代 $t = \ln v - 1$：

$$\ln v = 1 + C_1 x$$

$$v = e^{1+C_1 x}$$

回代 $p = vx$：

$$y' = xe^{1+C_1 x}$$

积分：

$$y = \int xe^{1+C_1 x},dx = e\int xe^{C_1 x},dx$$

$$= e\left[\frac{x}{C_1}e^{C_1 x} - \frac{1}{C_1^2}e^{C_1 x}\right] + C_2$$

$$\boxed{y = e^{1+C_1 x}!\left(\frac{x}{C_1} - \frac{1}{C_1^2}\right) + C_2}$$

---

# 习题二（pp.113）

## 第2题

证明：设 $f_1(x),\dots,f_k(x)$ 在 $(a,b)$ 内线性无关，则由这些函数构造的 $k$ 个新函数 $g_i(x) = \sum_{j=1}^k a_{ij}f_j(x)$（$i=1,\dots,k$）在 $(a,b)$ 内也线性无关的充要条件是系数行列式 $\det(a_{ij}) \neq 0$。

**证明：**

设 $\mathbf{g} = A\mathbf{f}$，其中 $A = (a_{ij})_{k\times k}$，$\mathbf{f} = (f_1,\dots,f_k)^T$，$\mathbf{g} = (g_1,\dots,g_k)^T$。

**必要性（$g_i$ 线性无关 $\Rightarrow$ $\det A \neq 0$）：**

反证。若 $\det A = 0$，则存在不全为零的 $c_1,\dots,c_k$ 使得 $\sum_i c_i \mathbf{a}_i = \mathbf{0}$（$\mathbf{a}_i$ 是 $A$ 的第 $i$ 行）。那么：

$$\sum_{i=1}^k c_i g_i(x) = \sum_{i=1}^k c_i \sum_{j=1}^k a_{ij}f_j(x) = \sum_{j=1}^k \left(\sum_{i=1}^k c_i a_{ij}\right)f_j(x) = 0$$

因为 $\sum_i c_i a_{ij} = 0$ 对每个 $j$ 成立。这说明 $g_i$ 线性相关，矛盾。

**充分性（$\det A \neq 0$ $\Rightarrow$ $g_i$ 线性无关）：**

设 $\sum_{i=1}^k c_i g_i(x) \equiv 0$，即：

$$\sum_{j=1}^k \left(\sum_{i=1}^k c_i a_{ij}\right)f_j(x) \equiv 0$$

因为 $f_1,\dots,f_k$ 线性无关，所以 $\sum_{i=1}^k c_i a_{ij} = 0$ 对所有 $j$ 成立。

写成矩阵形式：$\mathbf{c}^T A = \mathbf{0}^T$，即 $A^T \mathbf{c} = \mathbf{0}$。

因为 $\det A \neq 0$，所以 $\det A^T \neq 0$，故 $\mathbf{c} = \mathbf{0}$。

因此 $g_1,\dots,g_k$ 线性无关。$\blacksquare$

---

## 第4题

设 $y_i$（$i=1,\dots,n+1$）是 $n$ 阶非齐次线性方程 $L[y] = f(x)$ 的 $n+1$ 个线性无关的解，试求对应齐次方程 $L[y]=0$ 的基本解组，并求 $L[y]=f(x)$ 的通解。

**解：**

因为 $L[y_i] = f(x)$ 对每个 $i$ 成立，所以对任意两个解的差：
 
$$L[y_i - y_{n+1}] = L[y_i] - L[y_{n+1}] = f(x) - f(x) = 0$$

所以 $\eta_i = y_i - y_{n+1}$（$i = 1,\dots,n$）都是齐次方程的解。

**证明 $\eta_1,\dots,\eta_n$ 线性无关：**

若 $\sum_{i=1}^n c_i \eta_i \equiv 0$，即：

$$\sum_{i=1}^n c_i(y_i - y_{n+1}) \equiv 0$$

$$\sum_{i=1}^n c_i y_i - \left(\sum_{i=1}^n c_i\right) y_{n+1} \equiv 0$$

因为 $y_1,\dots,y_{n+1}$ 线性无关，所以 $c_1 = c_2 = \cdots = c_n = 0$ 且 $\sum c_i = 0$，这给出所有 $c_i = 0$。

因此 $\eta_1,\dots,\eta_n$ 构成齐次方程的**基本解组**。

**通解：** 取 $y_{n+1}$ 作为非齐次方程的一个特解，则：

$$\boxed{y = C_1(y_1 - y_{n+1}) + C_2(y_2 - y_{n+1}) + \cdots + C_n(y_n - y_{n+1}) + y_{n+1}}$$

---

## 第5题

设 $y_1(x),\dots,y_n(x)$ 是齐次方程 $y^{(n)} + p_1(x)y^{(n-1)} + \cdots + p_n(x)y = 0$ 的基本解组，$W(x)$ 是其朗斯基行列式。证明刘维尔公式：

$$W(x) = W(x_0)\exp!\left[-\int_{x_0}^x p_1(\xi),d\xi\right]$$

**证明：**

$W(x)$ 是 $n\times n$ 行列式，第 $i$ 行是 $(y_1^{(i-1)}, y_2^{(i-1)}, \dots, y_n^{(i-1)})$。

对 $W(x)$ 求导，根据行列式求导法则（逐行求导）：

$$W'(x) = \sum_{i=1}^n W_i(x)$$

其中 $W_i$ 是将第 $i$ 行求导、其余行不变的行列式。

当对第 $i$ 行（$i < n$）求导时，第 $i$ 行变成 $(y_1^{(i)}, \dots, y_n^{(i)})$，这恰好与第 $i+1$ 行相同，所以 $W_i = 0$（两行相同，行列式为零）。

**唯一非零项是 $W_n$：** 对最后一行（第 $n$ 行）求导，$(y_1^{(n-1)},\dots,y_n^{(n-1)})$ 变成 $(y_1^{(n)},\dots,y_n^{(n)})$。

由方程 $y_j^{(n)} = -p_1 y_j^{(n-1)} - p_2 y_j^{(n-2)} - \cdots - p_n y_j$，代入：

$$W' = \begin{vmatrix} y_1 & \cdots & y_n \ y_1' & \cdots & y_n' \ \vdots & & \vdots \ y_1^{(n-2)} & \cdots & y_n^{(n-2)} \ y_1^{(n)} & \cdots & y_n^{(n)} \end{vmatrix}$$

将最后一行 $y_j^{(n)} = -p_1 y_j^{(n-1)} - p_2 y_j^{(n-2)} - \cdots - p_n y_j$ 代入。

利用行列式的行线性性，最后一行分拆为 $n$ 项。其中含 $-p_2 y_j^{(n-2)}$ 的那项与第 $n-1$ 行相同，行列式为零；含 $-p_3 y_j^{(n-3)}$ 的与第 $n-2$ 行相同，也为零……以此类推，只有含 $-p_1 y_j^{(n-1)}$ 的项不与其他行重复。

因此：

$$W'(x) = -p_1(x) \cdot W(x)$$

这是关于 $W$ 的一阶线性方程，解为：

$$W(x) = W(x_0)\exp!\left[-\int_{x_0}^x p_1(\xi),d\xi\right] \qquad \blacksquare$$



# 第二部分：线性微分方程的一般理论

这部分偏理论，公式和定理比较多，但核心脉络其实很清晰。我按逻辑链条一步步来。

---

## 一、什么是线性微分方程？

$n$ 阶线性微分方程的标准形式：

$$y^{(n)} + p_1(x)y^{(n-1)} + \cdots + p_{n-1}(x)y' + p_n(x)y = f(x)$$

**"线性"的意思：** $y$ 和它的各阶导数都是一次的，没有 $y^2$、$yy'$、$(y')^3$ 这类东西。

引入一个记号，定义**线性微分算子**：

$$L[y] = y^{(n)} + p_1(x)y^{(n-1)} + \cdots + p_n(x)y$$

那方程就简写为 $L[y] = f(x)$。$f(x) \equiv 0$ 叫齐次，$f(x) \not\equiv 0$ 叫非齐次。

算子 $L$ 最重要的性质是**线性性**：

$$L[\alpha u + \beta v] = \alpha L[u] + \beta L[v]$$

这个性质贯穿后面所有理论。

---

## 二、存在唯一性定理

> **定理：** 若 $p_1(x), \dots, p_n(x), f(x)$ 在区间 $(a,b)$ 上连续，则对任意初值 $$y(x_0) = y_0,; y'(x_0) = y_1,; \dots,; y^{(n-1)}(x_0) = y_{n-1}$$ 方程在**整个** $(a,b)$ 上存在唯一解。

和一阶方程的存在唯一性比，有一个重要区别：线性方程的解**可以延展到整个系数连续的区间**，不会中途"爆掉"。这是线性结构带来的好处。

---

## 三、齐次方程的解空间——基本解组

先看齐次方程 $L[y] = 0$。

### 1. 解集构成线性空间

由线性性，如果 $y_1, y_2$ 是 $L[y]=0$ 的解，那 $C_1 y_1 + C_2 y_2$ 也是解。更一般地，解的任意线性组合还是解。

**关键结论：** $n$ 阶齐次线性方程的全部解构成一个 **$n$ 维线性空间**。

这意味着：只要找到 $n$ 个线性无关的解 $y_1, \dots, y_n$，齐次方程的通解就是：

$$y = C_1 y_1 + C_2 y_2 + \cdots + C_n y_n$$

这组 $y_1, \dots, y_n$ 就叫**基本解组**——本质上就是解空间的一组基。

### 2. 怎么判断线性无关？——朗斯基（Wronski）行列式

给定 $n$ 个函数 $y_1, \dots, y_n$，它们的**朗斯基行列式**定义为：

$$W(x) = \begin{vmatrix} y_1 & y_2 & \cdots & y_n \ y_1' & y_2' & \cdots & y_n' \ \vdots & \vdots & & \vdots \ y_1^{(n-1)} & y_2^{(n-1)} & \cdots & y_n^{(n-1)} \end{vmatrix}$$

就是把每个函数及其前 $n-1$ 阶导数排成一个矩阵，取行列式。

**核心判别定理：**

> 设 $y_1, \dots, y_n$ 是齐次方程 $L[y]=0$ 的 $n$ 个解。则它们构成基本解组 $\iff$ 存在某个 $x_0$ 使得 $W(x_0) \neq 0$。

而且，如果在某一点 $W(x_0) \neq 0$，那在**整个区间上处处不为零**。反过来，如果某一点 $W(x_0) = 0$，则处处为零。

也就是说：**$W$ 要么恒不为零，要么恒为零，不存在"有的地方为零有的地方不为零"的情况。**

⚠️ **重要提醒：** 这个"全或无"的性质**仅对同一个齐次方程的解组成立**。如果随便拿几个函数算朗斯基行列式，$W=0$ 并不一定能推出线性相关。这是很多人容易搞混的点。

### 3. 为什么 $W$ 有这么好的性质？——刘维尔（Liouville）公式

> $$W(x) = W(x_0)\exp!\left[-\int_{x_0}^x p_1(\xi),d\xi\right]$$

右边的指数函数**永远不为零**，所以 $W(x)$ 是否为零完全取决于 $W(x_0)$ 是否为零。这就解释了"全或无"的现象。

注意公式里只出现了 $p_1(x)$——就是方程中 $y^{(n-1)}$ 前面的系数。

**这个公式怎么来的？** 对 $W(x)$ 求导，用行列式逐行求导法则：把第 $i$ 行求导，其余行不变，加起来。前 $n-1$ 个行列式中，求导后的行与相邻行相同，所以为零。只有最后一行求导的那个不为零，再利用方程 $y_j^{(n)} = -p_1 y_j^{(n-1)} - \cdots$ 代入，最终得到 $W' = -p_1 W$，这是个一阶线性方程，解就是刘维尔公式。（你作业第5题要证的就是这个。）

---

## 四、非齐次方程的解结构

现在看 $L[y] = f(x)$。

### 1. 通解 = 齐次通解 + 一个特解

> **定理：** 设 $y^_$ 是 $L[y] = f(x)$ 的一个特解，$Y$ 是齐次方程 $L[y]=0$ 的通解，则非齐次方程的通解为： $$y = Y + y^_ = C_1 y_1 + \cdots + C_n y_n + y^*$$

**证明很简单：** 设 $y$ 是任意一个非齐次解，则 $L[y - y^_] = L[y] - L[y^_] = f - f = 0$，所以 $y - y^_$ 是齐次解。反过来，齐次解加上 $y^_$ 就是非齐次解。

这个结构你在一阶线性方程里其实已经见过了——通解公式 $y = Ce^{-\int p,dx} + \text{特解}$，本质上就是这个定理的特例。

### 2. 叠加原理

> 若 $L[y_1] = f_1(x)$，$L[y_2] = f_2(x)$，则 $L[y_1 + y_2] = f_1(x) + f_2(x)$。

如果右端 $f(x)$ 可以拆成几部分之和，可以分别求特解再加起来。

### 3. 一个推论：复解的实部和虚部

如果方程系数都是实的，复值函数 $y = u(x) + iv(x)$ 是解，那 $u$ 和 $v$ 分别也是解。

这在后面求常系数方程的通解时非常有用——复特征根给出复数形式的解，取实部虚部就得到两个实解。

---

## 五、把几个概念串起来

整个理论的逻辑链条是这样的：

$$\text{线性性} \xrightarrow{\text{推出}} \text{解集是线性空间（}n\text{维）}$$

$$\xrightarrow{\text{需要}} \text{找}n\text{个线性无关的解（基本解组）}$$

$$\xrightarrow{\text{怎么判断无关？}} \text{朗斯基行列式 }W \neq 0$$

$$\xrightarrow{\text{为什么}W\text{这么好用？}} \text{刘维尔公式：}W' = -p_1 W$$

而非齐次方程只多一步：找到基本解组之后，再加一个特解就行。

---

## 六、与你作业的对应关系

- **第2题**：线性无关函数经过线性变换后还无关吗？关键就是系数矩阵行列式是否为零——这是线性代数里的结论在函数空间里的直接应用。
    
- **第4题**：$n+1$ 个非齐次解，两两做差得齐次解，凑出基本解组——直接运用"非齐次解之差是齐次解"这个结构。
    
- **第5题**：证明刘维尔公式——就是对 $W$ 求导，用行列式逐行求导法则加方程代入。
    

---

要不要现在开始写作业？还是哪个地方还需要再展开讲讲？




![[Pasted image 20260528205721.png]]
# 2025年3月13日 常微分方程试题解答

## 第1题

$$\frac{dy}{dx} + y\tan x = \sin 2x, \quad x \in \left(-\frac{\pi}{2}, \frac{\pi}{2}\right)$$

一阶线性方程，$p(x) = \tan x$，$f(x) = \sin 2x = 2\sin x\cos x$。

积分因子 $\mu = e^{\int \tan x,dx} = e^{-\ln\cos x} = \frac{1}{\cos x} = \sec x$

（在 $(-\frac{\pi}{2},\frac{\pi}{2})$ 上 $\cos x > 0$，无绝对值问题。）

$$d(y\sec x) = \sec x \cdot 2\sin x\cos x,dx = 2\sin x,dx$$

$$y\sec x = -2\cos x + C$$

$$\boxed{y = -2\cos^2 x + C\cos x}$$

可以也写成 $y = -(1+\cos 2x) + C\cos x$。

---

## 第2题

$$\frac{dy}{dx} = e^{x+y} + e^{x-y}$$

$$y' = e^x e^y + e^x e^{-y} = e^x(e^y + e^{-y})$$

这是**可分离变量方程**：

$$\frac{dy}{e^y + e^{-y}} = e^x,dx$$

左边：$e^y + e^{-y} = 2\cosh y$，所以：

$$\frac{dy}{2\cosh y} = \frac{e^y,dy}{e^{2y}+1}$$

（分子分母同乘 $e^y$）

令 $t = e^y$，$dt = e^y,dy$：

$$\int \frac{dt}{t^2+1} = \arctan t = \arctan(e^y)$$

右边：

$$\int e^x,dx = e^x + C$$

所以：

$$\boxed{\arctan(e^y) = e^x + C}$$

---

## 第3题

$$y' + y\tan x = y^3\sec x$$

**伯努利方程**，$n = 3$。

两边除以 $y^3$（设 $y \neq 0$）：

$$y^{-3}y' + y^{-2}\tan x = \sec x$$

令 $z = y^{-2}$，$z' = -2y^{-3}y'$，所以 $y^{-3}y' = -\frac{1}{2}z'$：

$$-\frac{1}{2}z' + z\tan x = \sec x$$

$$z' - 2z\tan x = -2\sec x$$

一阶线性方程，积分因子 $\mu = e^{-\int 2\tan x,dx} = e^{2\ln\cos x} = \cos^2 x$：

$$d(z\cos^2 x) = -2\sec x \cdot \cos^2 x,dx = -2\cos x,dx$$

$$z\cos^2 x = -2\sin x + C$$

回代 $z = y^{-2}$：

$$\frac{\cos^2 x}{y^2} = -2\sin x + C$$

$$\boxed{\frac{\cos^2 x}{y^2} = C - 2\sin x}$$

另外 $y \equiv 0$ 也是解（代入原方程：$0 + 0 = 0$ ✓）。

---

## 第4题

$$y'' = y'e^y, \quad y(0) = 0,; y'(0) = 1$$

**不显含 $x$**，令 $p = y'$，$y'' = p\dfrac{dp}{dy}$：

$$p\frac{dp}{dy} = pe^y$$

若 $p \neq 0$，两边除以 $p$：

$$\frac{dp}{dy} = e^y$$

$$p = e^y + C_1$$

代入初值 $y(0)=0, y'(0)=1$，即 $y=0$ 时 $p=1$：

$$1 = e^0 + C_1 = 1 + C_1 \implies C_1 = 0$$

所以 $p = e^y$，即：

$$\frac{dy}{dx} = e^y$$

分离变量：

$$e^{-y}dy = dx$$

$$-e^{-y} = x + C_2$$

代入 $y(0)=0$：$-1 = 0 + C_2$，$C_2 = -1$。

$$-e^{-y} = x - 1$$

$$e^{-y} = 1 - x$$

$$\boxed{y = -\ln(1-x)}$$

（定义域 $x < 1$。）

---

## 第5题

$$y,dx - (x + y^2\cos y),dy = 0$$

写成 $M,dx + N,dy = 0$ 的形式：$M = y$，$N = -(x + y^2\cos y)$。

$M_y = 1$，$N_x = -1$，不相等，不是全微分。

**换个思路：** 把 $x$ 看成 $y$ 的函数，改写方程：

$$y\frac{dx}{dy} = x + y^2\cos y$$

$$\frac{dx}{dy} - \frac{1}{y}x = y\cos y$$

这是关于 $x(y)$ 的**一阶线性方程**！$p(y) = -\frac{1}{y}$，$f(y) = y\cos y$。

积分因子 $\mu = e^{\int -\frac{1}{y}dy} = e^{-\ln|y|} = \frac{1}{y}$（取 $y > 0$）：

$$d!\left(\frac{x}{y}\right) = \cos y,dy$$

$$\frac{x}{y} = \sin y + C$$

$$\boxed{x = y\sin y + Cy}$$

![[Pasted image 20260528205731.png]]

# 期中小测解答

## 第1题

$xy' + y - x^2y^2 = 0$

改写：$y' + \dfrac{1}{x}y = xy^2$

**伯努利方程**，$n=2$。两边除以 $y^2$（设 $y \neq 0$）：

$$y^{-2}y' + \frac{1}{x}y^{-1} = x$$

令 $z = y^{-1}$，$z' = -y^{-2}y'$：

$$-z' + \frac{1}{x}z = x$$

$$z' - \frac{1}{x}z = -x$$

积分因子 $\mu = e^{-\int \frac{1}{x}dx} = \frac{1}{x}$：

$$d!\left(\frac{z}{x}\right) = -dx$$

$$\frac{z}{x} = -x + C$$

回代 $z = \frac{1}{y}$：

$$\frac{1}{xy} = C - x$$

$$\boxed{y = \frac{1}{x(C - x)}}$$

另外 $y \equiv 0$ 也是解。

---

## 第2题

$xy,dy - (x^2 + y^2),dx = 0$，$y(1) = 0$

改写：

$$xy,dy = (x^2 + y^2),dx$$

$$\frac{dy}{dx} = \frac{x^2 + y^2}{xy} = \frac{x}{y} + \frac{y}{x}$$

**齐次方程**，令 $u = \dfrac{y}{x}$，$y = ux$，$y' = u + xu'$：

$$u + xu' = \frac{1}{u} + u$$

$$xu' = \frac{1}{u}$$

分离变量：

$$u,du = \frac{dx}{x}$$

$$\frac{u^2}{2} = \ln|x| + C$$

回代 $u = \frac{y}{x}$：

$$\frac{y^2}{2x^2} = \ln|x| + C$$

代入 $y(1) = 0$：$0 = 0 + C$，$C = 0$。

$$\frac{y^2}{2x^2} = \ln|x|$$

$$\boxed{y^2 = 2x^2\ln|x|}$$

---

## 第3题

$y''(1-y) = y'(1+y')$

**不显含 $x$**，令 $p = y'$，$y'' = p\dfrac{dp}{dy}$：

$$p\frac{dp}{dy}(1-y) = p(1+p)$$

若 $p \neq 0$，两边除以 $p$：

$$\frac{dp}{dy}(1-y) = 1 + p$$

$$\frac{dp}{dy} = \frac{1+p}{1-y}$$

分离变量：

$$\frac{dp}{1+p} = \frac{dy}{1-y}$$

$$\ln|1+p| = -\ln|1-y| + C$$

$$|1+p| = \frac{A}{|1-y|}$$

即：

$$1 + p = \frac{C_1}{1-y}$$

$$p = \frac{C_1}{1-y} - 1 = \frac{C_1 - 1 + y}{1-y}$$

即 $\dfrac{dy}{dx} = \dfrac{C_1 - 1 + y}{1 - y}$，分离变量：

$$\frac{(1-y),dy}{C_1 - 1 + y} = dx$$

令 $a = C_1 - 1$，分子改写 $1 - y = (a+1) - (a+y) = (a+1) - (a+y)$：

$$\frac{(1-y),dy}{a + y} = \frac{(a+1) - (a+y)}{a+y},dy = \left(\frac{a+1}{a+y} - 1\right)dy$$

积分：

$$(a+1)\ln|a+y| - y = x + C_2$$

回代 $a = C_1 - 1$：

$$\boxed{C_1\ln|C_1 - 1 + y| - y = x + C_2}$$

另外 $p = 0$ 即 $y = C$（常数）也是解。

---

## 第4题

探照灯反射镜面——截面曲线是抛物线。

**建模：** 设光源在原点，反射光线沿 $x$ 轴正方向平行射出。截面曲线为 $y = y(x)$。

由反射定律（入射角 = 反射角），光线从原点射到曲线上一点 $P(x,y)$，反射后平行于 $x$ 轴。

设切线与 $x$ 轴夹角为 $\alpha$，向径 $OP$ 与 $x$ 轴夹角为 $\theta$。反射定律要求切线是入射光与反射光的角平分线，由几何关系：

$$\tan\alpha = \frac{y}{x - x + y/\tan\alpha}$$

更直接的推导：从原点到 $P(x,y)$ 的光线方向向量为 $(x,y)$，反射光方向为 $(1,0)$。法线是角平分线方向，切线垂直于法线。

利用半角关系，设 $\tan\frac{\theta}{2}$ 其中 $\theta$ 是向径与 $x$ 轴的夹角，可以得到：

$$y' = \frac{y}{x + \sqrt{x^2+y^2}}$$

**或者用更简洁的方法：** 设焦点在原点，反射光平行于 $x$ 轴。对曲线上点 $(x,y)$，切线与 $x$ 轴夹角为 $\alpha = \arctan y'$。由反射几何：

$$\tan\alpha = \frac{y}{x}\cdot\frac{1}{1-\frac{y'y}{x}} \quad \text{(复杂)}$$

**用极坐标更清晰。** 设 $r, \theta$ 为极坐标，焦点在原点，反射光平行极轴。

反射条件：曲线在每一点的切线与向径的夹角等于切线与平行于极轴方向的夹角，即 $\psi = \frac{\pi - \theta}{2}$（其中 $\psi$ 是切线与向径的夹角）。

由公式 $\tan\psi = \frac{r}{dr/d\theta}$：

$$\frac{r}{r'} = \tan\frac{\pi-\theta}{2} = \cot\frac{\theta}{2} = \frac{\cos(\theta/2)}{\sin(\theta/2)}$$

分离变量：

$$\frac{dr}{r} = \frac{\sin(\theta/2)}{\cos(\theta/2)}d\theta$$

$$\ln r = -2\ln\cos\frac{\theta}{2} + C$$

$$r = \frac{a}{\cos^2(\theta/2)} = \frac{2a}{1+\cos\theta}$$

这正是以原点为焦点、开口向右的**抛物线**的极坐标方程。

转化为直角坐标（焦点在原点，准线 $x = -a$）：

$$\boxed{y^2 = 4a(x + a), \quad a > 0}$$

即截面曲线为**抛物线**。

---

## 第5题

试证：任意一个 $n$ 阶线性方程最多有 $n+1$ 个线性无关解。

**题意澄清：** 这里的"线性无关解"是指非齐次方程 $L[y] = f(x)$（$f \not\equiv 0$）的解。

**证明：**

设 $n$ 阶线性方程 $L[y] = f(x)$，$y_1, y_2, \dots, y_{n+2}$ 是它的 $n+2$ 个解。要证它们必线性相关。

作差：令 $\eta_i = y_{i+1} - y_1$，$i = 1, 2, \dots, n+1$。

则 $L[\eta_i] = L[y_{i+1}] - L[y_1] = f - f = 0$，所以 $\eta_1, \dots, \eta_{n+1}$ 都是**齐次方程** $L[y]=0$ 的解。

齐次方程的解空间是 $n$ 维的，因此 $n+1$ 个齐次解 $\eta_1, \dots, \eta_{n+1}$ 必线性相关，即存在不全为零的 $c_1, \dots, c_{n+1}$ 使得：

$$\sum_{i=1}^{n+1} c_i \eta_i \equiv 0$$

$$\sum_{i=1}^{n+1} c_i(y_{i+1} - y_1) \equiv 0$$

$$\sum_{i=1}^{n+1} c_i y_{i+1} - \left(\sum_{i=1}^{n+1} c_i\right)y_1 \equiv 0$$

令 $c_0 = -\sum_{i=1}^{n+1} c_i$，则：

$$c_0 y_1 + c_1 y_2 + c_2 y_3 + \cdots + c_{n+1} y_{n+2} \equiv 0$$

其中 $c_1, \dots, c_{n+1}$ 不全为零，所以 $c_0, c_1, \dots, c_{n+1}$ 也不全为零。

因此 $y_1, y_2, \dots, y_{n+2}$ 线性相关。

这说明非齐次方程不可能有 $n+2$ 个线性无关的解，即**最多有 $n+1$ 个线性无关解**。 $\blacksquare$

![[Pasted image 20260528205742.png]]
![[Pasted image 20260528205755.png]]
![[Pasted image 20260528205800.png]]
![[Pasted image 20260528205807.png]]
第一题：分开积分即可 

第二题：注意n为2，令z=y^1-2=y^-1，之后取微分代入后化简成一阶线性微分方程，两边乘以积分因子后用udv+vdu=d(uv)合并积分，（积分因子为e^积分z项前系数，这个操作主要是为了使得udv+vdu可以合并成d(uv)，对于一阶线性微分方程一般都可以这样做），另外由于令z=1/y时默认了y不为0的情况，所以要另外讨论y为0的时候是否为方程的解 

第三题：乘以积分因子后通过瞪眼法合并 

第四题：题目为f(x,y',y'')形式，令p(x)=y'就可降阶，之后利用udv+vdu=d(uv)合并积分即可