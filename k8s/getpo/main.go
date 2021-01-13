package main

import (
	"fmt"

	v1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/tools/clientcmd"
)

func main() {
	kubeconfig := "/Users/liyiyang/.kube/config"
	config, err := clientcmd.BuildConfigFromFlags("", kubeconfig)
	if err != nil {
		panic(err)
	}

	clientset, err := kubernetes.NewForConfig(config)
	if err != nil {
		panic(err)
	}

	pod, err := clientset.CoreV1().Pods("jobs").Get("lyy-debug-f26817fc-45b8-11eb-bb94-925dc5ef36d5", v1.GetOptions{})
	if err != nil {
		panic(err)
	}

	fmt.Println(pod.Spec.NodeName)
}
