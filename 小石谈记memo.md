# 我用 ClaudeCode 包揽论文全流程的心法分享 | Agent科研实践（一）

原创 小石谈记memo 小石谈记memo

 _2026年5月13日 11:59_ _浙江_ 听全文

![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/uiatxAtutsDbjXhBKUAWmnNtbK7KEDEl3do41VeYG6r6sibPHcqehEtpM17TJX76gHu89U1Iz7jQf3m3g2kDOFibNMyhtzF6zOhXoeI6X3NqxQ/640?wx_fmt=png&from=appmsg&wxfrom=13&tp=wxpic#imgIndex=0)

全文文字无AI生成内容，放心食用。

最近一直在用ClaudeCode（以下统称CC）来包揽辅助我科研和写论文，可以说CC从头到尾参与了整个流程，干的活比我干的多得多（这个月也就烧了48M的token吧，不过这统计不准，仅供参考）

在这个过程中，通过频繁踩坑和试错，也摸索出了一些很好用的心法和Prompt，可以让CC更能完全发挥其能力。在这里写一篇文章和大家分享。

![图片](https://mmbiz.qpic.cn/mmbiz_png/uiatxAtutsDbicgiaJVUmRJ0ITA8mYywfliaH6lz3gAuO9Ea2j4hR2ibyRzBbSnFlgchW6A96Qvuyoziagq8hulxbibBwa5H9DenWh1O5gjV6SYcC0/640?wx_fmt=png&from=appmsg&wxfrom=13&tp=wxpic#imgIndex=1)

 写在前面 

---

说实话，对于科研来说，如果你还没用过Agent来辅助，不管是CC还是Codex，我都非常推荐你去尝试一下，一开始肯定存在一些学习成本，但是用了之后你就知道相较各种网页对话体来说他们的效率有多高，有多值得你花一个下午的时间打破路径依赖学习一下怎么下载和配置。

开始之前你需要的工具：

- Claude code/Codex 或者其他Agent产品
    
- 任意一个聪明的大模型
    
- 几个可以用到的skill（后面会列出）
    

  

对于接下来分享的内容，我比较喜欢用心法这个词，而不是方法。因为掌握了心法是可以灵活使用在任何场景之下的，而不只是在某个特定的场景用某个方法。掌握心法往往比下载一堆花里胡哨的skill更重要。

另外这都是个人的经验分享，不构成任何建议。

---

  

 怎么从0开始建立你的项目 

确定好你的研究主题之后，首先要做的是和ai讨论清楚你的研究方法和路线。如果不确定能一次实现的预实验的部分也需要进行交代。

实践流程：

新建你的项目文件夹 ——> 开一个新session  ——> 告诉ai你的研究主题

这里可以尽量详细说明你的想法，但同时你需要让他不断对你进行提问，直到把所有不确定的因素都理清楚，这样可以防止ai自己臆想，导致跑偏。这里推荐一个skill和一个巨好用的提示词。

```
SKILL: Superpowers/brainstorming
```

虽然这个skill很多时候是用于项目开发的，但是用于理清需求非常有用。

```
Prompt：调用brainstorming skill跟我讨论【研究路线】，
```

这个提示词是卡兹克的一篇公众号文章里他分享的提问心法中提到的，实践下来非常好用，建议大家每次遇到描述不清楚自己的需求，或者自己都不知道自己想要什么的时候都用这个心法来进行提问。

AI能够帮你干好活的前提是至少你自己得知道你想让他干什么活。

和AI梳理清楚你的需求和研究方法之后，让它写成md文档放在文件夹中。

接下来，初始化你的项目  

——> 让CC根据你的研究方案为你建立清晰的文件夹和必要的配置文件 ——> 用/Init命令初始化项目的Claude.md

以上这两步是接下来AI能干好活所有活的前提条件。

一个清晰的项目文件夹和Claude.md/Agent.md能确保后面CC干活的时候不会浪费太多的上下文来寻找一些本应知晓的内容，从而导致影响输出质量。

到这里你已经完成了初步的基建工作，可以在这基础上开始你的科研实验流程了。

---

  

 心法：巧使markdown文档 妙理上下文 

其实让CC能够干好活，本质上是管理好大模型的上下文。

一个必须知道的知识点：很多时候任务完成的不理想，感觉大模型降智，很有可能是上下文管理不当，上下文过长或者过乱，导致Context Rot，也就是上下文腐烂。

很多人包括我之前一开始用大模型都会有一个问题：一开始某个项目的时候，打开一个session（会话框）进行提问之后，几乎从项目开始到项目结束，都不想换session，总觉得换个Session AI就再也不懂我了，很没有安全感。

结果就是，一个Session承受了太多不该承受的，方法讨论、Debug、换方法、写论文全都放在一个Session里。然后对话一长AI直接懵逼了，记忆混淆，回答变简单，bug乱修等等问题就来了。

应对心法1.1：尽量保持Session垂直度，一个Session就干一件事情；同一个项目下，有新任务就尽量新建新Session。

比如：数据处理的代码让A session写了，发现有bug，debug的任务可以让新的session来；出图结果不好，尽量不要让主Session修，也让新Session来，不要污染主对话。（主对话就是你用来执行最主要最重要任务的对话，也就是主要用来推进进度的那个对话）

原因很简单，保持上下文干净，AI才能发挥最强智力。

AI和人一样，你可以理解成不同的Session就是一个组织里不同的角色，你把自己看成Boss，剩下的Session都是给你打工的员工，主Session是组织的二把手，也就是最懂你的那个对话，你要做的是让二把手就干二把手的工作，不要让他一会去修bug，一会去读论文，换谁这么干都要罢工。琐碎的其他工作，都交给新的session，也就是组织的底层小员工，根据要求打打螺丝，简单修修bug就行。

这样才能保持效率最高，办事最靠谱。

![image](https://mmbiz.qpic.cn/sz_mmbiz_png/uiatxAtutsDYuzFZxcuXuicGsqxfW2p1Q0Gxlh6dPBOGnp0nFCg2psezfEMuqFmvic0CPsqHKhpNMJNKwNY2gErklK9jl7OwAx6yIayw9NRQ7I/640?wx_fmt=png&from=appmsg&tp=wxpic&wxfrom=5&wx_lazy=1#imgIndex=2)但是这样应运而生的就会有个问题，我新切个session，它不懂我了怎么办，不知道进度怎么办？我让新的Session进行工作，又要写一堆提示词怎么办？

应对心法1.2：用Markdown文档进行上下文管理，把要用的都让AI写下来

AI跟人一样，好记性不如烂笔头。。把经常需要用的研究计划、研究进度等等文档都存在文件夹中，让CC要用的时候进行阅读，让新Session在做任务前先进行阅读，能够快速了解项目背景。

接下来，分享几个我每次都会让AI写下来的markdown文档。

- Research Proposal.md
    
- Project_index.md
    
- Progress.md
    

接下来，逐个讲一下每个文档都是用来做什么的。

Research Proposal.md

这个文档很简单，就是在所有项目开始的时候，你和AI充分讨论得出的最终研究方案和研究路线，在AI记得最清楚的时候，让他理出来，固定在md文档中。这样无论是切换对话，或者要更新研究方法，只需要修改这个文档，再让AI读取就可以。

Project_index.md

这个文档是项目索引文档，在项目初始化之后或者需要的时候我会新开某个对话对整个项目进行梳理，把整个项目文件夹结构，包括每个脚本的作用，不同文件的数据流向，不同数据是用什么脚本处理的，最终得到的数据表格又放在哪里，都会记录在这个md文档中。

用来解决什么问题：1、给人看的；CC在执行任务过程中会新建很多脚本，一个一个翻代码去理解某个文件是用来做什么的，太花费时间，不如让CC自己整理好，教给你看。   2、给AI看的；新建一个Session，让他进行debug或者数据处理，找不到脚本/数据在哪怎么办，找错了怎么办？让他读这个文档，找到对应的数据或者脚本定向处理就好，一方面省上下文，另一方面也减小出错的概率。

Progress.md

我最最得意满意的实践过程中发现的非常实用的文档！也可以用progress.txt

这个文档是用来记录项目进度的，每次有重要进度，都让AI进行progress.md的更新。举个某次更新的例子：

```
## 2026年05月01日 23:00 - 整合Trimmed TWFE与生成综合报告
```

很简单，人做实验的时候人会忘记干到哪里了下一步要做什么，AI也会忘记。那么写个Progress.md让AI每次有进度就进行更新是最好的方法，同样在新开Session的时候，CC也可以对这个文档进行读取来了解现在到底干到什么程度了，能够了解到接下来到底要干什么。

![image](https://mmbiz.qpic.cn/sz_mmbiz_png/uiatxAtutsDbicbDcJLiabSweGQNMnHbj9eGeKmyzRMpBnO8RFMq2oTmqSJGwEaV4HRjIR2z6iaWfOzT16VkuAHC4ayZGHWb5IxZ1Wf4dibjhvlg/640?wx_fmt=png&from=appmsg&tp=wxpic&wxfrom=5&wx_lazy=1#imgIndex=3)

 心法：把重要原则都写进Claude.md 

有了以上的心法之后，你要做的就是教会AI做这些工作。

怎么教会CC呢，最好的方法就是写进Claude.md。

分享我某个工作的Claude.md的结构，内容不全，只是举例，大家可以参考。

```
# Project
```

建议大家在实践过程中逐渐完善自己的Claude.md，把重要的原则都写进去，这样CC不会忘记你的要求，会越用越好用。

  

 心法：最简单的交接办法 —— Handoff.md 

有了以上的基础，大家应该都明白新建Session的重要性和实践方法了。

分享我常用的新开Session的提示词，来直观了解怎么让新Session快速知道需要做什么。

```
请你根据claude.md阅读几个重要文档，快速了解当前项目状态。然后请你帮我（填入你的要求），先plan再进行工作
```

这样基本能够完成绝大部分的工作。

但是如果任务进行到一半，还没更新progress，上下文就有点爆了，不太放心怎么办？

让当前Session生成一个Handoff.md文档，用于交接任务。提示词：请你为我生成一个handoff文档，用于给下一个对话进行任务交接，让下一个对话能够正确接手你的任务。 大道至简，不用太多的提示，ai也可以做的很好。

然后就新开一个对话让他读这个文档进行工作就ok了。

---

到此，应该可以用CC辅助完成实验的绝大部分工作了，每一个内容都是我实践出来的心得，下一篇文章会讲讲怎么在这个基础上用CC来写出真正能用的高质量的论文。



